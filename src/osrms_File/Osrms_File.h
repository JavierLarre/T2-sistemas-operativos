#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define N_PROCESS 32
#define N_FILE 5


typedef struct osrms_file {
    int valid;
    char name[14];
    int size;
    int virtual_address;
} osrmsFile;


typedef struct process {
    int valid;
    int pid;
    char name[11];
    osrmsFile file_table[N_FILE];
    int first_level_page_table[64];
} Process;


typedef struct pcb_table {
    Process processes[N_PROCESS];
} PCB_Table;


typedef struct bitmap_tabla_paginas_segundo_orden {
    int bitmap[128];
} BitmapSO;


typedef struct espacio_tablas_segundo_orden {
    int tablas[1024][64];
} EspacioTablasSegundoOrden;


typedef struct frame_bitmap {
    int bitmap[8 * (1 << 10)]; // 8 * 1024 = 8KB
} FrameBitmap;


typedef struct frame {
    unsigned int PFN;
    int bytes[32 * (1 << 10)]; // 32 * 1024 = 32KB
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