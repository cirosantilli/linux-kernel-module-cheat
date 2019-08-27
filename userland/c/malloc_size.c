/* https://cirosantilli.com/linux-kernel-module-cheat#malloc
 *
 * Malloc n bytes as given from the command line.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    char *chars;
    size_t nbytes;

    if (argc < 2) {
        nbytes = 2;
    } else {
        nbytes = strtoull(argv[1], NULL, 0);
    }
    chars = malloc(nbytes);
    if (chars == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    free(chars);
    return EXIT_SUCCESS;
}
