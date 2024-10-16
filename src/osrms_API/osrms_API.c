#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "osrms_API.h"

void os_mount(char *memory_path) {
    set_memory_path(memory_path);
}


void os_ls_processes() {
    
}


int os_exists(int process_id, char *file_name) {
    return 0;
}


void os_ls_files(int process_id) {
    
}


void os_frame_bitmap() {
    
}

void os_tp_bitmap() {
    
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
    printf("Process name: %s\n", p->name);
    printf("Process pid: %d\n", p->pid);
    printf("Process address on memory: %d\n", p->address_on_memory);
}


void print_file(osrmsFile *f) {
    printf("File name: %s\n", f->name);
    printf("File size: %d\n", f->size);
    printf("File virtual address: %d\n", f->virtual_address);
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