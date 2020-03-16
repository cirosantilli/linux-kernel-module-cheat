/* https://cirosantilli.com/linux-kernel-module-cheat#m5ops-instructions */

#include <stdlib.h>

#define LKMC_M5OPS_ENABLE 1
#include <lkmc/m5ops.h>

int main(int argc, char **argv) {
    char action;
    unsigned long long loops;

    if (argc > 1) {
        action = argv[1][0];
    } else {
        action = 'e';
    }
    if (argc > 2) {
        loops = strtoull(argv[2], NULL, 0);
    } else {
        loops = 1;
    }
    for (unsigned long long i = 0; i < loops; i++) {
        switch (action) {
            case 'c':
                LKMC_M5OPS_CHECKPOINT;
                break;
            case 'd':
                LKMC_M5OPS_DUMPSTATS;
                break;
            case 'e':
                LKMC_M5OPS_EXIT;
                break;
            case 'f':
                LKMC_M5OPS_FAIL_1;
                break;
            case 'r':
                LKMC_M5OPS_RESETSTATS;
                break;
        }
    }
    return EXIT_SUCCESS;
}
