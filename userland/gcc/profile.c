/* https://cirosantilli.com/linux-kernel-module-cheat#profiling-userland-programs */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

uint64_t __attribute__ ((noinline)) common(uint64_t n, uint64_t seed) {
    for (uint64_t i = 0; i < n; ++i) {
        seed = (seed * seed) - (3 * seed) + 1;
    }
    return seed;
}

uint64_t __attribute__ ((noinline)) fast(uint64_t n, uint64_t seed) {
    uint64_t max = (n / 10) + 1;
    for (uint64_t i = 0; i < max; ++i) {
        seed = common(n, (seed * seed) - (3 * seed) + 1);
    }
    return seed;
}

uint64_t __attribute__ ((noinline)) maybe_slow(uint64_t n, uint64_t seed, int is_slow) {
    uint64_t max = n;
    if (is_slow) {
        max *= 10;
    }
    for (uint64_t i = 0; i < max; ++i) {
        seed = common(n, (seed * seed) - (3 * seed) + 1);
    }
    return seed;
}

int main(int argc, char **argv) {
    uint64_t n, seed;
    if (argc > 1) {
        n = strtoll(argv[1], NULL, 0);
    } else {
        n = 1;
    }
    if (argc > 2) {
        seed = strtoll(argv[2], NULL, 0);
    } else {
        seed = 0;
    }
    seed += maybe_slow(n, seed, 0);
    seed += fast(n, seed);
    seed += maybe_slow(n, seed, 1);
    seed += fast(n, seed);
    seed += maybe_slow(n, seed, 0);
    seed += fast(n, seed);
    printf("%" PRIX64 "\n", seed);
    return EXIT_SUCCESS;
}
