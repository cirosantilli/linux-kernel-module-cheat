/* https://cirosantilli.com/linux-kernel-module-cheat#mmap-map-anonymous */

#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int *is;
    size_t nbytes, nints;

    /* Decide how many ints to allocate. */
    if (argc < 2) {
        nints = 2;
    } else {
        nints = strtoull(argv[1], NULL, 0);
    }
    nbytes = nints * sizeof(*is);

    /* Allocate 2 ints. */
    is = mmap(
        NULL,
        nbytes,
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS,
        -1,
        0
    );

    /* This can happen for example if we ask for too much memory. */
    if (is == NULL) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    /* Write to and read from the allocated memory. */
    is[0] = 1;
    is[1] = 2;
    assert(is[0] == 1);
    assert(is[1] == 2);

    /* Free the allocated memory. */
    munmap(is, nbytes);

    return EXIT_SUCCESS;
}
