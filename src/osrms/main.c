#include "../osrms_API/osrms_API.h"

#include <stdio.h>


void events() {
    printf("exists: %d\n", os_exists(117, "facebook.png"));
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