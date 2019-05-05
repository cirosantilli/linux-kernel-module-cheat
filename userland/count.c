/* Count to infinity with 1 second sleep between each increment. */

#define _XOPEN_SOURCE 700
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
    unsigned long i = 0, max;
    if (argc > 1) {
        max = strtoul(argv[1], NULL, 10);
    } else {
        max = ULONG_MAX;
    }
    while (i < max) {
        printf("%lu\n", i);
        i++;
        sleep(1);
    }
}
