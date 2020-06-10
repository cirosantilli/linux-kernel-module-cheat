/* https://cirosantilli.com/linux-kernel-module-cheat#micro-benchmarks
 * https://cirosantilli.com/linux-kernel-module-cheat#c-busy-loop
 * https://cirosantilli.com/linux-kernel-module-cheat#benchmark-emulators-on-userland-executables */

#include <stdlib.h>

void __attribute__ ((noinline)) busy_loop(
    unsigned long long max,
    unsigned long long max2
) {
    for (unsigned long long i = 0; i < max2; i++) {
        for (unsigned long long j = 0; j < max; j++) {
            __asm__ __volatile__ ("" : "+g" (i), "+g" (j) : :);
        }
    }
}

int main(int argc, char **argv) {
    unsigned long long max, max2;
    if (argc > 1) {
        max = strtoll(argv[1], NULL, 0);
    } else {
        max = 1;
    }
    if (argc > 2) {
        max2 = strtoll(argv[2], NULL, 0);
    } else {
        max2 = 1;
    }
    busy_loop(max, max2);
}
