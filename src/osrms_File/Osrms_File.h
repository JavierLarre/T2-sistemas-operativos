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
#define FILE_MAX_SIZE 64*MB
#define VIRTUAL_MEMORY_SIZE 128*MB
#define PAGE_SIZE 32*KB
#define FRAME_SIZE 32*KB

extern char *MemoryPath;


typedef struct process {
    uint8_t valid; // 1 byte
    uint8_t pid; // 1 byte
    unsigned char name[11]; // 11 bytes
    uint16_t address_on_memory;
} Process; // total 13 bytes


typedef struct osrms_file {
    uint8_t valid; // 1 byte
    unsigned char name[14]; // 14 bytes
    uint32_t size; // 4 bytes
    uint32_t virtual_address; // 4 bytes
} osrmsFile; // total 23 bytes


char* get_memory_path();
void set_memory_path(const char *path);
FILE *get_memory_file();
void close_memory();

Process *buscar_proceso(int pid);
osrmsFile *buscar_archivo(Process *p, char *name);
Process **get_processes();
void free_processes(Process **processes);
osrmsFile **get_files(Process *p);
void free_files(osrmsFile **files);
uint8_t get_frame_bitmap_byte(int byte_index);
uint8_t get_table_bitmap_byte(int byte_index);
void save_process(Process *p);
void save_files(osrmsFile **files, Process *p);
uint16_t *get_fo_page_table(Process *p);
uint16_t *get_so_page_table(int so_page_table_number);
uint16_t get_vpn(uint32_t virtual_address);
uint8_t get_vpn1(uint16_t vpn);
uint8_t get_vpn2(uint16_t vpn);
uint32_t get_physical_address(Process *p, osrmsFile *f);
uint32_t *get_occupied_addres(Process *p);
uint32_t *get_file_sizes(Process *p);
uint32_t get_virtual_address(Process *p, uint32_t file_size);