/* https://github.com/cirosantilli/linux-kernel-module-cheat#gnu-gas-assembler-immediates */

#include "common.h"
LKMC_ENTRY
    mov x0, 1
    mov x0, 0x1
    mov x0, 1
    mov x0, 0x1
LKMC_EXIT
