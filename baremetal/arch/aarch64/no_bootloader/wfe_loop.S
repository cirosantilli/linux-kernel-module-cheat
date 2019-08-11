/* https://cirosantilli.com/linux-kernel-module-cheat#gem5-simulate-limit-reached */

#include <lkmc/m5ops.h>

.global lkmc_start
lkmc_start:
    wfe
    b lkmc_start
