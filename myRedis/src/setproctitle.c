// #include <stdio.h>
/* Initialize and populate SPT to allow a future setproctitle()
 * call.
 *
 * As setproctitle() basically needs to overwrite argv[0], we're
 * trying to determine what is the largest contiguous block
 * starting at argv[0] we can use for this purpose.
 *
 * As this range will overwrite some or all of the argv and environ
 * strings, a deep copy of these two arrays is performed.
 */
void spt_init(int argc, char *argv[]) {

    for (int i = 0; i < 3; i++)
    {
        printf("setproc");

    }
    
}