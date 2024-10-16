#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "osrms_API.h"


static const bool only_show_valid = true;


void os_mount(char *memory_path) {
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
    
}


void os_finish_process(int process_id) {
    
}


osrmsFile *os_open(int process_id, char *file_name, char mode) {
    return NULL;
}


int os_read_file(osrmsFile *file, char *dest) {
    return 0;
}


int os_write_file(osrmsFile *file, char *src) {
    return 0;
}

void os_close(osrmsFile *file) {
    
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