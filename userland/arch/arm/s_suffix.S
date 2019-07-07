/* https://cirosantilli.com/linux-kernel-module-cheat#arm-s-suffix */

#include <lkmc.h>

LKMC_PROLOGUE

    /* Result is 0, set beq. */
    movs r0, 0
    LKMC_ASSERT(beq)

    /* The opposite. */
    movs r0, 1
    LKMC_ASSERT(bne)

    /* mov without s does not set the status. */
    movs r0, 0
    mov r0, 1
    LKMC_ASSERT(beq)

    /* movs still moves... */
    mov r0, 0
    movs r0, 1
    LKMC_ASSERT_EQ(r0, =1)

    /* add: the result is 0. */
    mov r0, 1
    adds r0, -1
    LKMC_ASSERT(beq)

    /* add: result non 0. */
    mov r0, 1
    adds r0, 1
    LKMC_ASSERT(bne)

LKMC_EPILOGUE
