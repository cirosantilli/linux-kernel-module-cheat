/* https://cirosantilli.com/linux-kernel-module-cheat#mmap-map-anonymous
 *
 * Malloc n bytes as given from the command line.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
    char *chars;
    size_t nbytes;

    /* Decide how many ints to allocate. */
    if (argc < 2) {
        nbytes = 2;
    } else {
        nbytes = strtoull(argv[1], NULL, 0);
    }

    /* Allocate the bytes. */
    chars = mmap(
        NULL,
        nbytes,
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS,
        -1,
        0
    );

    /* This can happen for example if we ask for too much memory. */
    if (chars == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    /* Free the allocated memory. */
    munmap(chars, nbytes);

    return EXIT_SUCCESS;
}
