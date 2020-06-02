/* https://cirosantilli.com/linux-kernel-module-cheat#c
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* We use double because time_t can be either floating point or integer.
 * POSIX guarantees that it is an integer, we could use uintmax_t then. */
static long double get_nanos(void) {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return ts.tv_sec * 1e9 + ts.tv_nsec;
}

int main(int argc, char **argv) {
    long double start;
    unsigned long long int i, niters;

    if (argc > 1) {
        niters = strtoull(argv[1], NULL, 0);
    } else {
        niters = 3;
    }
    i = 0;
    start = get_nanos();
    while (1) {
        printf("%Lf\n", get_nanos() - start);
        i++;
        if (i == niters)
            break;
    }
    return EXIT_SUCCESS;
}
