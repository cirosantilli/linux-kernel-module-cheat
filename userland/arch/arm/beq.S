/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-beq-instruction */

#include "common.h"

LKMC_ENTRY

    /* Smaller*/
    mov r0, 1
    cmp r0, 2
    LKMC_ASSERT(ble)
    LKMC_ASSERT(blt)
    LKMC_ASSERT(bne)

    /* Equal. */
    mov r1, 0
    cmp r1, 0
    LKMC_ASSERT(beq)
    LKMC_ASSERT(bge)
    LKMC_ASSERT(ble)

    /* Greater. */
    mov r0, 2
    cmp r0, 1
    LKMC_ASSERT(bge)
    LKMC_ASSERT(bgt)
    LKMC_ASSERT(bne)

LKMC_EXIT
