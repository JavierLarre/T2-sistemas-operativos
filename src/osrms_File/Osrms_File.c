#include "Osrms_File.h"


static char *memory_path = NULL;


char* getMemoryPath() {
    return memory_path;
}


void setMemoryPath(char *path) {
    memory_path = calloc(strlen(path) + 1, sizeof(char));
    strcpy(memory_path, path);
}