/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-cset-instruction */

#include "common.h"

ENTRY
    /* Test values. */
    mov x19, 0
    mov x20, 1

    /* eq is true, set x21 = 1. */
    cmp x19, x19
    cset x21, eq
    ASSERT_EQ(x21, 1)

    /* eq is false, set x21 = 0. */
    cmp x19, x20
    cset x21, eq
    ASSERT_EQ(x21, 0)

    /* Same for ne. */
    cmp x19, x19
    cset x21, ne
    ASSERT_EQ(x21, 0)

    cmp x19, x20
    cset x21, ne
    ASSERT_EQ(x21, 1)
EXIT
