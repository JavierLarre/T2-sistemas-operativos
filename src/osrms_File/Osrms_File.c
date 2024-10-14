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


void write_byte(uint32_t address, uint8_t data) {
    fseek(file, address, SEEK_SET);
    fwrite(&data, sizeof(uint8_t), 1, file);
}