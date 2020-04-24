/* https://cirosantilli.com/linux-kernel-module-cheat#getpid */

#define _XOPEN_SOURCE 700
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
    printf("pid=%jd\n", (intmax_t)getpid());
    return EXIT_SUCCESS;
}
