/* https://cirosantilli.com/linux-kernel-module-cheat#malloc */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/* We do this in a separate function just to illustrate that
 * this is allows for malloc memory! This is unlike regular stack
 * variables which may be deallocated when the function returns. */
void *allocate_bytes(size_t nbytes) {
    return malloc(nbytes);
}

int main(int argc, char **argv) {
    int *is;
    size_t nbytes, nints;

    /* Decide how many ints to allocate.
     * Unlike usual non-VLA arrays, the size is determined dynamically at runtime! */
    if (argc < 2) {
        nints = 2;
    } else {
        nints = strtoull(argv[1], NULL, 0);
    }
    nbytes = nints * sizeof(*is);
    is = allocate_bytes(nbytes);

    /* This can happen for example if we ask for too much memory. */
    if (is == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    /* Write to and read from the allocated memory. */
    is[0] = 1;
    is[1] = 2;
    assert(is[0] == 1);
    assert(is[1] == 2);

    /* Free the allocated memory. */
    free(is);

    return EXIT_SUCCESS;
}
