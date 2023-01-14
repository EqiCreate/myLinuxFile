#if (defined __linux || defined __APPLE__)
#define USE_SETPROCTITLE
#define INIT_SETPROCTITLE_REPLACEMENT
void spt_init(int argc, char *argv[]);
#endif
#ifdef __linux__
#include <features.h>
#include <fcntl.h>
#endif