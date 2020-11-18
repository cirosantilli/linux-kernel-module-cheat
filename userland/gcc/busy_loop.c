/* https://cirosantilli.com/linux-kernel-module-cheat#microbenchmarks
 * https://cirosantilli.com/linux-kernel-module-cheat#c-busy-loop
 * https://cirosantilli.com/linux-kernel-module-cheat#benchmark-emulators-on-userland-executables */

#include <lkmc.h>

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
    lkmc_busy_loop(max, max2);
}
