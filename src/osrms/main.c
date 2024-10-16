#include "../osrms_API/osrms_API.h"

#include <stdio.h>

int main(int argc, char const *argv[])
{
    os_mount((char *)argv[1]);
    os_ls_processes();
    close_memory();
    return 0;

}