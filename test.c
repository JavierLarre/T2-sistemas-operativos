#include "src/osrms_API/osrms_API.h"


// gcc -c src/osrms_API/osrms_API.c -o obj/osrms_API/osrms_API.o
// gcc -c src/osrms_File/Osrms_File.c -o obj/osrms_File/Osrms_File.o
// gcc -c test.c -o obj/test.o
// gcc obj/test.o obj/osrms_File/Osrms_File.o obj/osrms_API/osrms_API.o -o test


int main(int argc, char const *argv[])
{
    os_mount((char *)argv[1]);
    print_sizes();
    getchar();
    Process *p = buscar_proceso(0x75);
    printf("Proceso: %d\n", p->pid);
    printf("Nombre Proceso: %s\n", p->name);
    printf("Valid: %d\n", p->valid);
    free(p);
    return 0;
}