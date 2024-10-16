#include "../osrms_API/osrms_API.h"

#include <stdio.h>

int main(int argc, char const *argv[])
{

    // montar la memoria
    os_mount((char *)argv[1]);

    // debug
    // check if file is open

    if (get_file() == NULL) {
        printf("Error: No se pudo abrir el archivo de memoria\n");
        return 1;
    }

    // check all segments size
    // print pcb size in kb
    printf("pcb_table size: %lu KB\n", sizeof(pcb_table) / 1024);
    // print bitmap_so size in bytes
    printf("bitmap_so size: %lu bytes\n", sizeof(bitmap_so));
    // print tablas_so size in kb
    printf("tablas_so size: %lu KB\n", sizeof(espacio_tablas_so) / 1024);
    // print frame_bitmap size in kb
    printf("frame_bitmap size: %lu KB\n", sizeof(frame_bitmap) / 1024);
    // print memory size in GB
    printf("memory size: %lu GB\n", sizeof(memory) / 1024 / 1024 / 1024);

    // read pcb_table

    rw_pcb_table();

    // list all processes
    printf("Testing os_ls_processes\n");
    os_ls_processes();

    // list all files of process every process
    printf("Testing os_ls_files\n");
    for (int i = 0; i < N_PROCESS; i++) {
        Process *p = &pcb_table.processes[i];
        if (!p->valid) continue;
        int pid = p->pid;
        os_ls_files(pid);
    }

    // test os_exists
    printf("Testing os_exists\n");
    int exist = os_exists(228, "amogus.mp4"); // This exists
    if (exist) {
        printf("File exists\n");
    } else {
        printf("File does not exist. This is wrong.\n");
    }
    os_exists(228, "sus.mp4");    // This does not exist

    // read bitmap_so
    rw_bitmap_so();

    // check if bitmap was correctly read
    // first print bitmap from file
    // read starting from 8kb to 8kb + 128 bytes in file

    // fseek(file, sizeof(PCB_Table), SEEK_SET);

    // print 128 lines of 1 byte each. Each line is shown as a stream of 1s and 0s

    //for (int i = 0; i < 128; i++)
    //{
    //    uint8_t byte;
    //    fread(&byte, sizeof(uint8_t), 1, file);
    //    for (int j = 0; j < 8; j++)
    //    {
    //        printf("%d", (byte >> j) & 1);
    //    }
    //    printf(" Value of byte: %d\n", byte);
    //    printf("\n");
    //}
    
    // print bitmap_so from memory
    printf("Testing os_tp_bitmap\n");
    os_tp_bitmap();

    // read tablas_so

    rw_tablas_so();

    // read frame_bitmap

    rw_frame_bitmap();

    // check if frame_bitmap was correctly read
    // first print bitmap from file

    //fseek(file, sizeof(PCB_Table) + sizeof(BitmapSO) + sizeof(EspacioTablasSegundoOrden), SEEK_SET);

    // Bitmap is 8KB, so we have 2**16 bits and 2**13 bytes
    // If we print a line per byte, we will have 2**13 lines, that's a lot
    // We'll print a line per 8 bytes, so we'll have 2**10 lines
    // Reading is in little endian
    //for (int i = 0; i < 1024; i++)
    //{
    //    uint64_t byte_64;
    //    fread(&byte_64, sizeof(uint64_t), 1, file);
    //    for (int j = 0; j < 64; j++)
    //    {
    //        printf("%ld", (byte_64 >> j) & 1);
    //    }
    //    printf("\n");
    //}

    // print frame_bitmap from memory
    printf("Testing os_frame_bitmap\n");
    os_frame_bitmap();
    
    // start a process with pid 122 and name "tprocess"

    printf("Testing os_start_process\n");

    os_start_process(122, "tprocess");
    // search for process 122

    Process *p = buscar_proceso(122);

    if (p == NULL)
    {
        printf("Process not found\n");
    }

    else
    {
        printf("Process 122 found\n");
        printf("Process name: %s\n", p->name);
        printf("Process pid: %d\n", p->pid);
    }

    // finish process 122

    printf("Testing os_finish_process\n");

    os_finish_process(122);

    // search for process 122

    Process *p_2 = buscar_proceso(122);

    if (p_2 == NULL)
    {
        printf("Process not found\n");
    }

    else
    {
        printf("Process 122 found\n");
        printf("Process name: %s\n", p_2->name);
        printf("Process pid: %d\n", p_2->pid);
    }

    // open a file from process 117
    printf("Testing os_open of existent file\n");
    osrmsFile *file_117 = os_open(117, "dino.jpg", 'r');

    // print data to check if it was correctly read

    printf("File 'dino' data:\n");
    printf("Valid: %d\n", file_117->valid);
    printf("Name: %s\n", file_117->name);
    printf("Size: %d\n", file_117->size);
    printf("Virtual Address: %d\n", file_117->virtual_address);

    // open a file that does not exist
    printf("Testing os_open of non-existent file\n");
    osrmsFile *file_117_2 = os_open(117, "sus.mp4", 'r');

    if (file_117_2 == NULL)
    {
        printf("File does not exist\n");
    }

    else
    {
        printf("File exists\n");
    }

    // Try to write a file to a process with no space. 117 is full.
    printf("Testing os_open. Writing a file to a full process\n");
    osrmsFile *file_117_3 = os_open(117, "sus.mp4", 'w');
    if (file_117_3 == NULL)
    {
        printf("Process is full\n");
    }

    else
    {
        printf("Process has space\n");
    }

    // write a file to a process with space. 234 has 1 space left.
    printf("Testing os_open. Writing a file to a process with space\n");
    
    os_ls_files(234);
    
    osrmsFile *file_234 = os_open(234, "sus.mp4", 'w');
    
    if (file_234 == NULL)
    {
        printf("Process is full\n");
    }

    else
    {
        printf("Process has been written\n");
        os_ls_files(234);
    }

    // read file from memory

    return 0;

}