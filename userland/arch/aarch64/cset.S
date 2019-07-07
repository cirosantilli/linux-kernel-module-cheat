/* https://cirosantilli.com/linux-kernel-module-cheat#arm-cset-instruction */

#include <lkmc.h>

LKMC_PROLOGUE
    /* Test values. */
    mov x19, 0
    mov x20, 1

    /* eq is true, set x21 = 1. */
    cmp x19, x19
    cset x21, eq
    LKMC_ASSERT_EQ(x21, =1)

    /* eq is false, set x21 = 0. */
    cmp x19, x20
    cset x21, eq
    LKMC_ASSERT_EQ(x21, =0)

    /* Same for ne. */
    cmp x19, x19
    cset x21, ne
    LKMC_ASSERT_EQ(x21, =0)

    cmp x19, x20
    cset x21, ne
    LKMC_ASSERT_EQ(x21, =1)
LKMC_EPILOGUE
