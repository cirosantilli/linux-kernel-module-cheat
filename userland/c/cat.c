#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <lkmc/file_io.h>

int main(int argc, char **argv) {
    assert(argc == 2);
    char *s = lkmc_file_read(argv[1]);
    puts(s);
    free(s);
    return EXIT_SUCCESS;
}
