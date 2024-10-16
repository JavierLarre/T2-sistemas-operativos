#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "osrms_API.h"

void os_mount(char *memory_path) {
    set_memory_path(memory_path);
}


void os_ls_processes() {
    for (int i = 0; i < N_PROCESS; i++) {
        if (!pcb_table.processes[i].valid) 
            continue;
        printf("Proceso: %d\n", pcb_table.processes[i].pid);
        printf("Nombre Proceso: %s\n", pcb_table.processes[i].name);
        //printf("Valid: %d\n", pcb_table.processes[i].valid);
        printf("\n");
    }
}


int os_exists(int process_id, char *file_name) {
    Process *p = buscar_proceso(process_id);
    osrmsFile *f = buscar_archivo(p, file_name);
    if (f == NULL) {
        printf("Archivo %s no existe en proceso %d\n", file_name, process_id);
        return 0;
    }
    return f->valid;
}


// void os_ls_files(int process_id) {
//     Process *p = buscar_proceso(process_id);

//     if (!p->valid || p == NULL) {
//         printf("Proceso %d no existe\n", process_id);
//         return;
//     }

//     printf("Archivos del proceso %d:\n", process_id);
//     for (int i = 0; i < N_FILE; i++) {
//         if (p->file_table[i].valid) {
//             printf("\t%s: %d B\n", p->file_table[i].name, p->file_table[i].size);
//         }
//     }
//     printf("\n");
// }


void os_frame_bitmap() {
    int ocupados = 0;
    int libres = 0;
    int bit;
    uint64_t byte_64;
    printf("Frame Bitmap:\n");
    for (int i = 0; i < (1 << 10); i++) {
        byte_64 = frame_bitmap.bitmap[i];
        for (int j = 0; j < 64; j++) {
            bit = (byte_64 >> j) & 1;
            printf("%d", bit);
            if (bit) {
                ocupados++;
            } else {
                libres++;
            }
        }
        printf("\n");
    }
    printf("Frames Ocupados: %d\n", ocupados);
    printf("Frames Libres: %d\n", libres);

    // Debug: Suma de ocupados y libres debe ser 2^16
    int total = ocupados + libres;

    printf("Total: %d\n", total);
    printf("%s\n", total == (1 << 16) ? "Correcto" : "Incorrecto");

    printf("\n");
}

void os_tp_bitmap() {
    int ocupados = 0;
    int libres = 0;
    uint8_t byte;
    printf("Bitmap Tablas de Páginas de Segundo Orden:\n");
    // read bytes in little endian
    for (int i = 0; i < 128; i++) {
        byte = bitmap_so.bitmap[i];
        for (int j = 0; j < 8; j++) {
            printf("%d", (byte >> j) & 1);
            if ((byte >> j) & 1) {
                ocupados++;
            } else {
                libres++;
            }
        }
        printf("\n");
    }
    printf("Tablas de Páginas de Segundo Orden Ocupadas: %d\n", ocupados);
    printf("Tablas de Páginas de Segundo Orden Libres: %d\n", libres);
    // Debug: Suma de ocupados y libres debe ser 1024
    printf("Total: %d\n", ocupados + libres);
    printf("\n");
}


// void os_start_process(int process_id, char *process_name) {
//     bool found = false;
//     int index = -1;
//     //p.file_table = calloc(N_FILE, sizeof(osrmsFile));
    

//     for (int i = 0; i < N_PROCESS; i++) {
//         if (!pcb_table.processes[i].valid) {
//             pcb_table.processes[i].valid = 1;
//             pcb_table.processes[i].pid = process_id;
//             strcpy((char*)pcb_table.processes[i].name, process_name);
//             found = true;
//             index = i;
//             break;
//         }
//     }

//     if (!found) {
//         printf("ERROR: proceso %d no iniciado, no queda espacio disponible", process_id);
//         return;
//     }

//     for (int i = 0; i < N_FILE; i++) 
//         pcb_table.processes[index].file_table[i].valid = 0;
//     for (int i = 0; i < 64; i++) 
//         pcb_table.processes[index].first_level_page_table[i] = 0;
    
// }


void os_finish_process(int process_id) {
    for (int i = 0; i < N_PROCESS; i++) {
        if (pcb_table.processes[i].pid == process_id) {
            pcb_table.processes[i].valid = 0;
            break;
        }
    }
    
}


// osrmsFile *os_open(int process_id, char *file_name, char mode) {
//     Process *p = buscar_proceso(process_id);

//     if (!p->valid || p == NULL) return NULL;

//     osrmsFile *f = buscar_archivo(p, file_name);

//     if (mode == 'r') return f;
//     if (mode != 'w') return NULL;
//     //if (!f) return NULL;
//     for (int i = 0; i < N_FILE; i++) {
//         if (p->file_table[i].valid) continue;
//         p->file_table[i].valid = 1;
//         strcpy((char*)p->file_table[i].name, file_name);
//         p->file_table[i].size = 0;
//         p->file_table[i].virtual_address = 0;
//         return &p->file_table[i];
//     }
//     return NULL;
// }


uint32_t os_read_file(osrmsFile *file, char *dest) {
    uint32_t bytes_leidos = 0;
    FILE *f = fopen(dest, "wb");
    for (uint32_t i = 0; i < file->size; i++) {
        uint8_t byte;
        read_byte(file->virtual_address + i, &byte);
        // TODO: ESTE NO ES EL METODO CORRECTO
        // AQUI QUEDÉ
        fwrite(&byte, sizeof(uint8_t), 1, f);
        bytes_leidos++;
    } 
    fclose(f);
    return bytes_leidos;
}


int os_write_file(osrmsFile *file, char *src) {
    uint32_t bytes_escritos = 0;
    FILE *f = fopen(src, "rb");
    while (1) {
        uint8_t byte;
        fread(&byte, sizeof(uint8_t), 1, f);
        bool written = write_byte(file->virtual_address + bytes_escritos, byte);
        if (!written) {
            fclose(f);
            return bytes_escritos;
        }
        bytes_escritos++;
    }
}

void os_close(osrmsFile *file) {
    file->valid = 0;
    file->size = 0;
    file->virtual_address = 0;
    // ESTO NO ES TODO, FALTA LIBERAR MEMORIA
}

// Leer pcb del archivo de memoria "file", iniciar procesos y guardar la información en struct pcb_table
void rw_pcb_table() {

    fseek(file, 0, SEEK_SET);
    fread(&pcb_table, sizeof(PCB_Table), 1, file);
}
// bitmap empieza en la dirección 8kb y ocupa 128 bytes
void rw_bitmap_so() {
    fseek(file, sizeof(PCB_Table), SEEK_SET);
    fread(&bitmap_so, sizeof(BitmapSO), 1, file);
}

void rw_tablas_so() {
    fseek(file, sizeof(PCB_Table) + sizeof(BitmapSO), SEEK_SET);
    fread(&espacio_tablas_so, sizeof(EspacioTablasSegundoOrden), 1, file);
}

void rw_frame_bitmap() {
    fseek(file, sizeof(PCB_Table) + sizeof(BitmapSO) + sizeof(EspacioTablasSegundoOrden), SEEK_SET);
    fread(&frame_bitmap, sizeof(FrameBitmap), 1, file);
}

void rw_memory() {
    FILE *f = get_file();
    fseek(f, sizeof(PCB_Table) + sizeof(BitmapSO) + sizeof(EspacioTablasSegundoOrden) + sizeof(FrameBitmap), SEEK_SET);
    fread(&memory, sizeof(Memory), 1, f);
}