#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>


#define KB (1 << 10)
#define MB (1 << 20)
#define GB (1 << 30)


#define N_PROCESS 32
#define N_FILE 5


extern char *MemoryPath;


typedef struct process {
    uint8_t valid; // 1 byte
    uint8_t pid; // 1 byte
    unsigned char name[11]; // 11 bytes
    uint16_t address_on_memory;
} Process; // total 13 bytes


#pragma pack(1) // gonza q es eto
typedef struct osrms_file {
    uint8_t valid; // 1 byte
    unsigned char name[14]; // 14 bytes
    uint32_t size; // 4 bytes
    uint32_t virtual_address; // 4 bytes
} osrmsFile; // total 23 bytes


char* get_memory_path();
void set_memory_path(char *path);
void close_memory();

Process *buscar_proceso(int pid);
osrmsFile *buscar_archivo(Process *p, char *name);
Process **get_processes();