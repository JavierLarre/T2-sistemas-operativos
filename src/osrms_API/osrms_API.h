#pragma once
#include "../osrms_File/Osrms_File.h"


void os_mount(const char *memory_path);

void os_ls_processes();

int os_exists(int process_id, char *file_name);

void os_ls_files(int process_id);

void os_tp_bitmap();

void os_frame_bitmap();

void os_start_process(int process_id, char *process_name);

void os_finish_process(int process_id);

osrmsFile *os_open(int process_id, char *file_name, char mode);

int os_read_file(osrmsFile *file_desc, char *dest);

int os_write_file(osrmsFile *file_desc, char *src);

void os_close(osrmsFile *file_desc);

void print_process(Process *p);

void print_file(osrmsFile *f);