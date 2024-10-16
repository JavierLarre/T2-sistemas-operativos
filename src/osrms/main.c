#include "../osrms_API/osrms_API.h"

#include <stdio.h>


void events() {
    os_frame_bitmap();
    os_tp_bitmap();
}


int main(int argc, char const *argv[])
{
    FILE *log_file = freopen("output.log", "w", stdout);
    os_mount("mem_filled.bin");
    events();
    close_memory();
    fclose(log_file);
    return 0;

}