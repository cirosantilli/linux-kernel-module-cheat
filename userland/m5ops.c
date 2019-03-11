/* https://github.com/cirosantilli/linux-kernel-module-cheat#m5ops-instructions */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "m5ops.h"

int main(int argc, char **argv) {
    char action;
    if (argc > 1) {
        action = argv[1][0];
    } else {
        action = 'e';
    }
    switch (action) {
        case 'c':
            m5_checkpoint();
            break;
        case 'd':
            m5_dumpstats();
            break;
        case 'e':
            m5_exit();
            break;
        case 'f':
            m5_fail_1();
            break;
        case 'r':
            m5_resetstats();
            break;
    }
    return EXIT_SUCCESS;
}
