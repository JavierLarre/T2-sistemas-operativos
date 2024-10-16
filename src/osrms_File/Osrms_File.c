#include "Osrms_File.h"



char *MemoryPath = NULL;
FILE *file = NULL;


static bool compare_unsigned_string(unsigned char *s1, char *s2, size_t n) {
    return !strncmp((char*) s1, s2, n);
}


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


void close_memory() {
    if (MemoryPath != NULL) {
        free(MemoryPath);
    }
    if (file != NULL) {
        fclose(file);
    }
}


// uint32_t calcular_direccion_fisica(int pid, char *archivo) {
//     Process *p = buscar_proceso(pid);
//     osrmsFile *f = buscar_archivo(p, archivo);
//     uint32_t offset = f->virtual_address && 0x7fff; // 15 bits menos significativos
//     uint32_t vpn = f->virtual_address >> 15;
//     uint32_t sptn = p->first_level_page_table[vpn >> 6];
//     uint32_t pfn = espacio_tablas_so.tablas[sptn][vpn && 0x3f];
//     return pfn | offset;
// }


Process *buscar_proceso(int pid) {
    Process *p = calloc(1, sizeof(Process));
    const size_t tamano_proceso = sizeof(Process);
    const size_t tamano_en_memoria = 256;
    unsigned int base_address = 0;
    for (int i = 0; i < N_PROCESS; i++) {
        fseek(file, base_address, SEEK_SET);
        fread(p, tamano_proceso, 1, file);
        p->address_on_memory = base_address;
        if (p->valid && p->pid == pid) return p;
        base_address += tamano_en_memoria;
    }
    free(p);
    return NULL;
}


bool file_is_valid(osrmsFile *f, char *name) {
    return f->valid && compare_unsigned_string(f->name, name, sizeof(f->name));
}


osrmsFile *buscar_archivo(Process *p, char *name) {
    osrmsFile *osrms_file = calloc(1, sizeof(osrmsFile));
    const size_t tamano_archivo = sizeof(osrmsFile);
    uint16_t base_address = p->address_on_memory + 13;
    fseek(file, base_address, SEEK_SET);
    for (int i = 0; i < N_FILE; i++) {
        fseek(file, i * tamano_archivo, SEEK_CUR);
        fread(osrms_file, tamano_archivo, 1, file);
        if (file_is_valid(osrms_file, name)) {
            return osrms_file;
        }
    }
    free(osrms_file);
    return NULL;
}


Process **get_processes() {
    Process **processes = calloc(N_PROCESS, sizeof(Process*));
    const size_t tamano_proceso = 13;
    const size_t tamano_en_memoria = 256;
    unsigned int base_address = 0;
    for (int i = 0; i < N_PROCESS; i++) {
        processes[i] = calloc(1, sizeof(Process));
        fseek(file, base_address, SEEK_SET);
        fread(processes[i], tamano_proceso, 1, file);
        processes[i]->address_on_memory = base_address;
        base_address += tamano_en_memoria;
    }
    return processes;
}


osrmsFile **get_files(Process *p) {
    osrmsFile **files = calloc(N_FILE, sizeof(osrmsFile*));
    const size_t tamano_archivo = sizeof(osrmsFile);
    uint16_t base_address = p->address_on_memory + 13;
    for (int i = 0; i < N_FILE; i++) {
        files[i] = calloc(1, sizeof(osrmsFile));
        fseek(file, base_address, SEEK_SET);
        fread(files[i], tamano_archivo, 1, file);
        base_address += tamano_archivo - 1;
        // -1 to avoid the fseek in the next iteration
        // because the next iteration will do the fseek
        // in the correct position
        // copilot comentó esto
        // QUE FSEEK HACE QUE
    }
    return files;
}


uint8_t get_frame_bitmap_byte(int byte_index) {
    uint8_t byte;
    const uint32_t base_address = 8*KB + 128 + 128*KB;
    fseek(file, base_address + byte_index, SEEK_SET);
    fread(&byte, sizeof(uint8_t), 1, file);
    return byte;
}


uint8_t get_table_bitmap_byte(int byte_index) {
    uint8_t byte;
    const uint32_t base_address = 8*KB;
    fseek(file, base_address + byte_index, SEEK_SET);
    fread(&byte, sizeof(uint8_t), 1, file);
    return byte;
}


void save_process(Process *p) {
    fseek(file, p->address_on_memory, SEEK_SET);
    fwrite(p, 13, 1, file);
}