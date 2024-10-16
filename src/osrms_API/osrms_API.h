#pragma once
#include "../osrms_File/Osrms_File.h"


void print_sizes();
void os_mount(char *memory_path);
void os_ls_processes();
int os_exists(int process_id, char *file_name);
void os_ls_files(int process_id);
void os_tp_bitmap();
void os_frame_bitmap();
void os_start_process(int process_id, char *process_name);
void os_finish_process(int process_id);
osrmsFile *os_open(int process_id, char *file_name, char mode);
uint32_t os_read_file(osrmsFile *file, char *dest);
int os_write_file(osrmsFile *file, char *src);
void os_close(osrmsFile *file);

void rw_pcb_table();
void rw_bitmap_so();
void rw_tablas_so();
void rw_frame_bitmap();
void rw_memory();