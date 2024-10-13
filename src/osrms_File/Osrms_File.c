#include "Osrms_File.h"


static char *memory_path = NULL;
static FILE *file = NULL;


char* get_memory_path() {
    return memory_path;
}


FILE *get_file() {
    return file;
}


void set_memory_path(char *path) {
    if (memory_path != NULL) {
        free(memory_path);
    }
    memory_path = calloc(strlen(path) + 1, sizeof(char));
    strcpy(memory_path, path);
    
    if (file != NULL) {
        fclose(file);
    }
    file = fopen(memory_path, "rb+");
}


void read_byte(uint32_t address, uint8_t *data) {
    fseek(file, address, SEEK_SET);
    fread(data, sizeof(uint8_t), 1, file);
}


void write_byte(uint32_t address, uint8_t data) {
    fseek(file, address, SEEK_SET);
    fwrite(&data, sizeof(uint8_t), 1, file);
}