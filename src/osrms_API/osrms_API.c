#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "osrms_API.h"


static const bool only_show_valid = false;


void os_mount(const char *memory_path) {
    set_memory_path(memory_path);
}


void os_ls_processes() {
    Process **processes = get_processes();
    for (int i = 0; i < N_PROCESS; i++) {
        if (only_show_valid && !processes[i]->valid) {
            free(processes[i]);
            continue;
        }
        printf("Process %d\n", i);
        print_process(processes[i]);
        free(processes[i]);
    }
    free(processes);
}


int os_exists(int process_id, char *file_name) {
    Process *p = buscar_proceso(process_id);
    osrmsFile *file = buscar_archivo(p, file_name);
    free(p);
    return file != NULL;
}


void os_ls_files(int process_id) {
    Process *p = buscar_proceso(process_id);
    osrmsFile **files = get_files(p);
    free(p);
    for (int i = 0; i < N_FILE; i++) {
        if (only_show_valid && !files[i]->valid) {
            free(files[i]);
            continue;
        }
        printf("File %d\n", i);
        print_file(files[i]);
        free(files[i]);
    }
    free(files);
}


static int print_binary(uint8_t number) {
    int ocupados = 0;
    for (int i = 7; i >= 0; i--) {
        char bit = (number & (1 << i)) ? '1' : '0';
        printf("%c", bit);
        if (bit == '1') ocupados++;
    }
    printf("\n");
    return ocupados;
}


void os_frame_bitmap() {
    const uint32_t base_address = 8*KB + 128 + 128*KB;
    int ocupados = 0;
    printf("Frame bitmap:\n");
    for (int i = 0; i < (8*KB); i++) {
        uint8_t byte = get_frame_bitmap_byte(i);
        printf("0x%x ", base_address + i);
        ocupados += print_binary(byte);
    }
    printf("\n");
    printf("Ocupados: %d\tLibres: %d\n", ocupados, 8*KB - ocupados);
    printf("\n");
}

void os_tp_bitmap() {
    const uint32_t base_address = 8*KB;
    int ocupados = 0;
    printf("Table bitmap:\n");
    for (int i = 0; i < 128; i++) {
        uint8_t byte = get_table_bitmap_byte(i);
        printf("0x%x ", base_address + i);
        ocupados += print_binary(byte);
    }
    printf("\n");
    printf("Ocupados: %d\tLibres: %d\n", ocupados, 128 - ocupados);
    printf("\n");
}


void os_start_process(int process_id, char *process_name) {
    Process **processes = get_processes();
    for (int i = 0; i < N_PROCESS; i++) {
        if (processes[i]->valid)
            continue;
        processes[i]->valid = 1;
        processes[i]->pid = process_id;
        strcpy((char*) processes[i]->name, process_name);
        save_process(processes[i]);
        break;
    }
    free_processes(processes);
}


void os_finish_process(int process_id) {
    Process **processes = get_processes();
    FILE *memory = get_memory_file();
    for (int i = 0; i < N_PROCESS; i++) {
        if (processes[i]->pid == process_id) {
            processes[i]->valid = 0;
            save_process(processes[i]);
            break;
        }
    }
    free_processes(processes);
    // TODO: liberar memoria ocupada
}


osrmsFile *os_open(int process_id, char *file_name, char mode) {
    
    if (mode != 'r' && mode != 'w') return NULL;
    Process *p = buscar_proceso(process_id);
    if (p == NULL) return NULL; 
    osrmsFile *p_file = buscar_archivo(p, file_name);
    if (mode == 'r') {
        free(p);
        return p_file;
    }
    if (p_file) {
        // File exists
        free(p);
        free(p_file);
        return NULL; 
    }
    osrmsFile **files = get_files(p);
    for (int i = 0; i < N_FILE; i++) {
        if (file_is_valid(files[i]))
            continue;
        p_file = files[i];
        p_file->valid = 1;
        strcpy((char*) p_file->name, file_name);
        p_file->size = 0;
        p_file->virtual_address = 0; // 0 For the moment
        break;
    }
    if (p_file == NULL) // File table is full
        return NULL;
    save_files(files, p);
    free_files(files);
    return p_file;
}

// Read the file data from memory, save it in a new file and return the amount of bytes read
int os_read_file(osrmsFile *file_desc, char *dest) {
    
    Process *p = get_process_from_file(file_desc);
    if (p == NULL) {
        printf("Given file does not exist\n");
        return 0;
    }
    int total_read_bytes = 0;
    int current_page = 0;
    FILE *memory = get_memory_file();

    // Open a new file to save the data

    FILE *dest_file = fopen(dest, "w");

    // Get virtual address of the file
    uint32_t virtual_address = file_desc->virtual_address;
    uint32_t file_size = file_desc->size;
    uint16_t offset = virtual_address & 0x7FFF;  // 15 least significant bits are the offset
    
    uint32_t current_page_read_bytes = 0;
    uint32_t current_page_offset = 0;
    uint32_t current_page_left_bytes_to_read = PAGE_SIZE - offset;

    uint32_t bytes_left_to_read = file_size;

    uint32_t physical_address = get_physical_address(p, file_desc);

    while (bytes_left_to_read > 0) {
        if (current_page_left_bytes_to_read == 0) {
            current_page++;
            current_page_offset = 0;
            current_page_read_bytes = 0;
            current_page_left_bytes_to_read = PAGE_SIZE;
            physical_address = get_physical_address(p, file_desc);
        }

        fseek(memory, physical_address + current_page * PAGE_SIZE + current_page_offset, SEEK_SET);
        uint8_t byte;
        fread(&byte, sizeof(uint8_t), 1, memory);
        fwrite(&byte, sizeof(uint8_t), 1, dest_file);
        current_page_offset++;
        current_page_read_bytes++;
        current_page_left_bytes_to_read--;
        bytes_left_to_read--;
        total_read_bytes++;
        
    }

    fclose(dest_file);
    return total_read_bytes;
}

// Write the data from the source file to the file in memory
int os_write_file(osrmsFile *file_desc, char *src) {
    if (!file_desc) return 0;

    // find the process that owns the file
    Process **processes = get_processes();

    Process *p = NULL;

    for (int i = 0; i < N_PROCESS; i++) {
        if (!processes[i]->valid) continue;
        osrmsFile *file = buscar_archivo(processes[i], (char*) file_desc->name);
        if (file) {
            p = processes[i];
            free(file);
            break;
        }
    }
    if (!p) {
        printf("Given file does not exist\n");
        free_processes(processes);
        return 0;
    }

    FILE *memory = get_memory_file();
    FILE *src_file = fopen(src, "r");

    // get size of the source file

    fseek(src_file, 0, SEEK_END);
    uint32_t src_file_size = ftell(src_file);

    // check if the file is too big to be stored in memory

    if (src_file_size > FILE_MAX_SIZE) {
        printf("File is too big to be stored in memory\n");
        fclose(src_file);
        free_processes(processes);
        return 0;
    }

    // get source file name

    char *src_file_name = strrchr(src, '/');

    // assign name to the file descriptor

    strcpy((char*) file_desc->name, src_file_name);


    // Get virtual address of the file
    // This is a new file, so the virtual address is not set
    // The virtual address must be set searching for free space in virtual memory

    uint32_t virtual_address = get_virtual_address(p, src_file_size);

    if (virtual_address == -1) {
        printf("There is not enough space in the virtual memory to store the file\n");
        fclose(src_file);
        free_processes(processes);
        return 0;
    }

    uint32_t file_size = file_desc->size;
    uint16_t offset = virtual_address & 0x7FFF;  // 15 least significant bits are the offset
    
    // write variables
    uint32_t current_page = 0;
    uint32_t current_page_offset = 0;
    uint32_t current_page_left_bytes_to_write = PAGE_SIZE - offset;
    uint32_t bytes_left_to_write = file_size;
    uint32_t total_written_bytes = 0;

    uint32_t physical_address = get_physical_address(p, file_desc);
    int frame_number = physical_address / FRAME_SIZE;
    // start writing the file

    while (bytes_left_to_write > 0) {
        // check if current frame is occupied using bitmap

        uint8_t byte = get_frame_bitmap_byte(frame_number / 8);

        if (byte & (1 << (frame_number % 8))) {
            printf("Frame %d is occupied. Can't keep writing\n", frame_number);
            fclose(src_file);
            free_processes(processes);
            return 0;
        }

        if (current_page_left_bytes_to_write == 0) {
            current_page++;
            current_page_offset = 0;
            current_page_left_bytes_to_write = PAGE_SIZE;
            physical_address = get_physical_address(p, file_desc);
        }

        fseek(src_file, total_written_bytes, SEEK_SET);
        fread(&byte, sizeof(uint8_t), 1, src_file);

        fseek(memory, physical_address + current_page * PAGE_SIZE + current_page_offset, SEEK_SET);
        fwrite(&byte, sizeof(uint8_t), 1, memory);

        current_page_offset++;
        current_page_left_bytes_to_write--;
        bytes_left_to_write--;
        total_written_bytes++;
    }

    fclose(src_file);
    free_processes(processes);
    return total_written_bytes;

}

void os_close(osrmsFile *file_desc) {
    
    if (!file_desc) return;
    
    file_desc->valid = 0;
    // find the process that owns the file

    Process **processes = get_processes();

    Process *p = NULL;

    for (int i = 0; i < N_PROCESS; i++) {
        if (!processes[i]->valid) continue;
        osrmsFile *file = buscar_archivo(processes[i], (char*) file_desc->name);
        if (file) {
            p = processes[i];
            free(file);
            break;
        }
    }

    if (!p) {
        printf("Given file does not exist\n");
        free_processes(processes);
        return;
    }

    osrmsFile **files = get_files(p);
    
    save_files(files, p);

    free_processes(processes);
    free_files(files);
    free(file_desc);

}


void print_process(Process *p) {
    if (!only_show_valid) printf("\tis valid: %d\n", p->valid);
    printf("\tname: %s\n", p->name);
    printf("\tpid: %u\n", p->pid);
    printf("\taddress on memory: 0x%x\n", p->address_on_memory);
    printf("\n");
}


void print_file(osrmsFile *f) {
    if (!only_show_valid) printf("\tis valid: %d\n", f->valid);
    printf("\tFile name: %s\n", f->name);
    printf("\tFile size: %u\n", f->size);
    printf("\tFile virtual address: 0x%x\n",f->virtual_address);
}










// Leer pcb del archivo de memoria "file", iniciar procesos y guardar la información en struct pcb_table
// void rw_pcb_table() {

//     fseek(file, 0, SEEK_SET);
//     fread(&pcb_table, sizeof(PCB_Table), 1, file);
// }
// // bitmap empieza en la dirección 8kb y ocupa 128 bytes
// void rw_bitmap_so() {
//     fseek(file, sizeof(PCB_Table), SEEK_SET);
//     fread(&bitmap_so, sizeof(BitmapSO), 1, file);
// }

// void rw_tablas_so() {
//     fseek(file, sizeof(PCB_Table) + sizeof(BitmapSO), SEEK_SET);
//     fread(&espacio_tablas_so, sizeof(EspacioTablasSegundoOrden), 1, file);
// }

// void rw_frame_bitmap() {
//     fseek(file, sizeof(PCB_Table) + sizeof(BitmapSO) + sizeof(EspacioTablasSegundoOrden), SEEK_SET);
//     fread(&frame_bitmap, sizeof(FrameBitmap), 1, file);
// }

// void rw_memory() {
//     FILE *f = get_file();
//     fseek(f, sizeof(PCB_Table) + sizeof(BitmapSO) + sizeof(EspacioTablasSegundoOrden) + sizeof(FrameBitmap), SEEK_SET);
//     fread(&memory, sizeof(Memory), 1, f);
// }