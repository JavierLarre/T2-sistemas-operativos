#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "osrms_API.h"


void os_mount(char *memory_path) {
    set_memory_path(memory_path);
}