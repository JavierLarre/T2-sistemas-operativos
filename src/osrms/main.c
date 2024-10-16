#include "../osrms_API/osrms_API.h"

#include <stdio.h>

int main(int argc, char const *argv[])
{
    os_mount((char *)argv[1]);
    Process *p = buscar_proceso(0x75);
    osrmsFile *f = buscar_archivo(p, "dino.jpg");
    print_process(p);
    print_file(f);
    free(p);
    free(f);
    close_memory();
    return 0;

}