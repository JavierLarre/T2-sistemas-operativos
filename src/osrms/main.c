#include "../osrms_API/osrms_API.h"

#include <stdio.h>


void events() {
    os_ls_processes();
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