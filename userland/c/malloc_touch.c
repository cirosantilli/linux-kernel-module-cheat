/* https://cirosantilli.com/linux-kernel-module-cheat#gem5-memory-latency  */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    size_t nbytes, step;
    if (argc > 1) {
        nbytes = strtoull(argv[1], NULL, 0);
    } else {
        nbytes = 0x10;
    }
    if (argc > 2) {
        step = strtoull(argv[2], NULL, 0);
    } else {
        step = 1;
    }

    char *base = malloc(nbytes);
    assert(base);
    char *i = base;
    while (i < base + nbytes) {
        *i = 13;
        i += step;
    }
    return EXIT_SUCCESS;
}
