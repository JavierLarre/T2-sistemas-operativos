#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define N_PROCESS 32
#define N_FILE 5

#pragma pack(1)
typedef struct osrms_file {
    bool valid; // 1 byte
    unsigned char name[14]; // 14 bytes
    uint32_t size; // 4 bytes
    uint32_t virtual_address; // 4 bytes
} osrmsFile; // total 23 bytes


typedef struct process {
    uint8_t valid; // 1 byte
    uint8_t pid; // 1 byte
    unsigned char name[11]; // 11 bytes
    osrmsFile file_table[N_FILE]; // 5 * 23 = 115 bytes
    uint16_t first_level_page_table[64]; // 64 * 2 = 128 bytes
} Process; // total 256 bytes


typedef struct pcb_table {
    Process processes[N_PROCESS]; // 32 * 256 = 8KB
} PCB_Table; // total 8KB


typedef struct bitmap_tabla_paginas_segundo_orden {
    bool bitmap[128];
} BitmapSO;


typedef struct espacio_tablas_segundo_orden {
    uint16_t tablas[1024][64]; // Espacio total: 1024 * 64 * 2 = 128KB
} EspacioTablasSegundoOrden;


typedef struct frame_bitmap {
    uint64_t bitmap[8 * (1 << 10)]; // 8 bytes (64 bits) x celda. Total 1024 celdas
    // uint8_t bitmap[1 << 10]; // Tamaño total: 8kb
} FrameBitmap;


typedef struct frame {
    uint16_t PFN;
    uint8_t bytes[32 * (1 << 10)]; // 32 * 1024 = 32KB
} Frame;


typedef struct memory {
    Frame frames[1 << 16];
} Memory;

#pragma pack()

extern char *MemoryPath;
extern FILE *file;

FILE *get_file();

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
Process *buscar_proceso(int pid);
osrmsFile *buscar_archivo(Process *p, char *name);