#include "../osrms_API/osrms_API.h"

#include <stdio.h>


void events() {
    os_ls_processes();
    os_start_process(1, "process1");
    os_ls_processes();
    os_ls_files(210);

    osrmsFile *day = os_open(210, "day.png", 'r');
    
    printf("Test os_open\n");

    int dummy = os_read_file(day, "day.png");
    os_close(day);
    printf("Bytes read: %d\n", dummy);
}

int main(int argc, char const *argv[])
{
    FILE *log_file = freopen("output.log", "w", stdout);
    os_mount(argv[1]);
    events();
    close_memory();
    fclose(log_file);
    return 0;

}