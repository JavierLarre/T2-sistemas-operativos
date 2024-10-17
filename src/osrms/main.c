#include "../osrms_API/osrms_API.h"

#include <stdio.h>


void events() {
    os_ls_processes();
    os_ls_files(117);
    printf("size: %zu\n", sizeof(osrmsFile));
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