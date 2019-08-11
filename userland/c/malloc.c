/* https://cirosantilli.com/linux-kernel-module-cheat#malloc */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    size_t bytes = sizeof(int) * 2;
    /* Allocate 2 ints. */
    int *is = malloc(bytes);
    /* This can happen for example if we ask for too much memory. */
    if (is == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    is[0] = 1;
    assert(is[0] == 1);
    /* Free the allocated memory. */
    free(is);
    return EXIT_SUCCESS;
}

