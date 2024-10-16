#include "Osrms_File.h"


char *MemoryPath = NULL;
FILE *file = NULL;

// Memoria
PCB_Table pcb_table;
BitmapSO bitmap_so;
FrameBitmap frame_bitmap;
EspacioTablasSegundoOrden espacio_tablas_so;
Memory memory;


char* get_memory_path() {
    return MemoryPath;
}

void set_memory_path(char *path) {
    if (MemoryPath != NULL) {
        free(MemoryPath);
    }
    MemoryPath = calloc(strlen(path) + 1, sizeof(char));
    strcpy(MemoryPath, path);
    
    if (file != NULL) {
        fclose(file);
    }
    file = fopen(MemoryPath, "rb+");
}


FILE *get_file() {
    return file;
}


void read_byte(uint32_t address, uint8_t *data) {
    fseek(file, address, SEEK_SET);
    fread(data, sizeof(uint8_t), 1, file);
}


bool write_byte(uint32_t address, uint8_t data) {
    fseek(file, address, SEEK_SET);
    fwrite(&data, sizeof(uint8_t), 1, file);
    return true;
}


Process *buscar_proceso(int pid) {
    for (int i = 0; i < N_PROCESS; i++) {
        if (pcb_table.processes[i].pid == pid && pcb_table.processes[i].valid) {
            return &pcb_table.processes[i];
        }
    }
    return NULL;
}


osrmsFile *buscar_archivo(Process *p, char *name) {
    for (int i = 0; i < N_FILE; i++) {
        if (!p->file_table[i].valid) continue;
        if (strcmp((char*)p->file_table[i].name, name) == 0) {
            return &p->file_table[i];
        }
    }
    printf("Archivo %s no encontrado\n", name);
    return NULL;
}


uint32_t calcular_direccion_fisica(int pid, char *archivo) {
    Process *p = buscar_proceso(pid);
    osrmsFile *f = buscar_archivo(p, archivo);
    uint32_t offset = f->virtual_address && 0x7fff; // 15 bits menos significativos
    uint32_t vpn = f->virtual_address >> 15;
    uint32_t sptn = p->first_level_page_table[vpn >> 6];
    uint32_t pfn = espacio_tablas_so.tablas[sptn][vpn && 0x3f];
    return pfn | offset;
}
