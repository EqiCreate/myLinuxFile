// #include "server.h"
#include "config.h"
#include <time.h>
#include <sys/time.h>


int main(int argc, char **argv) {
    // struct timeval tv;
    int j;
    char config_from_stdin = 0;
    printf("mainserver %d",argc);
        /* We need to initialize our libraries, and the server configuration. */

#ifdef INIT_SETPROCTITLE_REPLACEMENT
    spt_init(argc, argv);
#endif
    tzset(); /* Populates 'timezone' global. */

}