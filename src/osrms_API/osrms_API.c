#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "osrms_API.h"


osrmsFile buscar_archivo(Process p, char *name) {
    for (int i = 0; i < N_FILE; i++) {
        if (strcmp(p.file_table[i].name, name) == 0) {
            return p.file_table[i];
        }
    }
    osrmsFile f;
    f.valid = 0;
    return f;
}


Process buscar_proceso(int pid) {
    for (int i = 0; i < N_PROCESS; i++) {
        if (pcb_table.processes[i].pid == pid) {
            return pcb_table.processes[i];
        }
    }
    Process p;
    p.valid = 0;
    return p;
}


int calcular_direccion_fisica(int pid, char *archivo) {
    Process p = buscar_proceso(pid);
    osrmsFile f = buscar_archivo(p, archivo);
    int offset = f.virtual_address && 0x7fff; // 15 bits menos significativos
    int vpn = f.virtual_address >> 15;
    int sptn = p.first_level_page_table[vpn >> 6];
    int pfn = espacio_tablas_so.tablas[sptn][vpn && 0b111111];
    return pfn | offset;
}


void os_mount(char *memory_path) {
    set_memory_path(memory_path);
}


void os_ls_processes() {
    for (int i = 0; i < N_PROCESS; i++) {
        if (!pcb_table.processes[i].valid) 
            continue;
        printf("Proceso: %d\n", pcb_table.processes[i].pid);
        printf("Nombre Proceso: %s\n", pcb_table.processes[i].name);
        printf("\n");
    }
}


int os_exists(int process_id, char *file_name) {
    Process p = buscar_proceso(process_id);
    osrmsFile f = buscar_archivo(p, file_name);
    return f.valid;
}


void os_ls_files(int process_id) {
    Process p = buscar_proceso(process_id);
    printf("Archivos del proceso %d:\n", process_id);
    for (int i = 0; i < N_FILE; i++) {
        if (p.file_table[i].valid) {
            printf("\t%s: %d\n", p.file_table[i].name, p.file_table[i].size);
        }
    }
    printf("\n");
}


void os_frame_bitmap() {
    int ocupados = 0;
    int libres = 0;
    printf("Frame Bitmap:\n");
    for (int i = 0; i < (1 << 10); i++) {
        for (int j = 0; j < 8; j++) {
            printf("%d", frame_bitmap.bitmap[i+j]);
            if (frame_bitmap.bitmap[i+j]) {
                ocupados++;
            } else {
                libres++;
            }
        }
        printf("\n");
    }
    printf("Frames Ocupados: %d\n", ocupados);
    printf("Frames Libres: %d\n", libres);
    printf("\n");
}


void os_tp_bitmap() {
    int ocupados = 0;
    int libres = 0;
    printf("Bitmap Tablas de Páginas de Segundo Orden:\n");
    for (int i = 0; i < (128 >> 3); i++) {
        for (int j = 0; j < 8; j++) {
            printf("%d", bitmap_so.bitmap[i+j]);
        }
        printf("\n");
    }
    printf("Tablas de Páginas de Segundo Orden Ocupadas: %d\n", ocupados);
    printf("Tablas de Páginas de Segundo Orden Libres: %d\n", libres);
    printf("\n");
}


void os_start_process(int process_id, char *process_name) {
    Process p;
    p.valid = 1;
    p.pid = process_id;
    strcpy(p.name, process_name);
    for (int i = 0; i < N_FILE; i++) 
        p.file_table[i].valid = 0;
    for (int i = 0; i < 64; i++) 
        p.first_level_page_table[i] = 0;
    for (int i = 0; i < N_PROCESS; i++) {
        if (!pcb_table.processes[i].valid) {
            pcb_table.processes[i] = p;
            break;
        }
    }
    printf("ERROR: proceso %d no iniciado, no queda espacio disponible", process_id);
}


void os_finish_process(int process_id) {
    for (int i = 0; i < N_PROCESS; i++) {
        if (pcb_table.processes[i].pid == process_id) {
            pcb_table.processes[i].valid = 0;
            break;
        }
    }
    // TODO: Liberar memoria asignada al proceso
}