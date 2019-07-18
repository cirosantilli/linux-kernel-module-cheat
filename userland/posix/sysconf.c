/* https://github.com/cirosantilli/linux-kernel-module-cheat#sysconf */

#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    /* Maximum lengh of command line arguments + environment variables: */
    printf("_SC_ARG_MAX (MiB) = %ld\n", sysconf(_SC_ARG_MAX) / (1 << 20));
    return EXIT_SUCCESS;
}
