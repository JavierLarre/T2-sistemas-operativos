#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define N_PROCESS 32
#define N_FILE 5


typedef struct osrms_file {
    bool valid;
    unsigned char name[14];
    uint32_t size;
    uint32_t virtual_address;
} osrmsFile;


typedef struct process {
    uint8_t valid;
    uint8_t pid;
    unsigned char name[11];
    osrmsFile file_table[N_FILE];
    uint16_t first_level_page_table[64];
} Process;


typedef struct pcb_table {
    Process processes[N_PROCESS];
} PCB_Table;


typedef struct bitmap_tabla_paginas_segundo_orden {
    bool bitmap[128];
} BitmapSO;


typedef struct espacio_tablas_segundo_orden {
    uint16_t tablas[1024][64];
} EspacioTablasSegundoOrden;


typedef struct frame_bitmap {
    bool bitmap[8 * (1 << 10)]; // 8 * 1024 = 8KB
    // uint8_t bitmap[1 << 10]; // 1024 bytes, cada byte tiene 8 bits, 1024 * 8 = 8KB
} FrameBitmap;


typedef struct frame {
    uint16_t PFN;
    uint8_t bytes[32 * (1 << 10)]; // 32 * 1024 = 32KB
} Frame;


typedef struct memory {
    Frame frames[1 << 16];
} Memory;


extern char *MemoryPath;
extern FILE *file;


// Memoria
extern PCB_Table pcb_table;
extern BitmapSO bitmap_so;
extern FrameBitmap frame_bitmap;
extern EspacioTablasSegundoOrden espacio_tablas_so;
extern Memory memory;

char* get_memory_path();
void set_memory_path(char *path);
void read_byte(uint32_t address, uint8_t *data);
bool write_byte(uint32_t address, uint8_t data);
Process buscar_proceso(int pid);
osrmsFile buscar_archivo(Process p, char *name);