/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-gnu-instruction-gas-assembler-immediates */

#include "common.h"

LKMC_ENTRY
    /* This is the default. We hack it in common.h however. */
.syntax divided
   /* These fail. */
#if 0
    mov r0, 1
    mov r0, 0x1
#endif
    mov r0, #1
    mov r0, #0x1
    mov r0, $1
    mov r0, $0x1
.syntax unified
    mov r0, 1
    mov r0, 0x1
    mov r0, 1
    mov r0, 0x1
    mov r0, $1
    mov r0, $0x1
LKMC_EXIT
