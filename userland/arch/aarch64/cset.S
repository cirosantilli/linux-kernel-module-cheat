/* https://github.com/cirosantilli/arm-assembly-cheat#cset */

#include "common.h"

ENTRY
    /* Test values. */
    mov x0, 0
    mov x1, 1

    /* eq is true, set x2 = 1. */
    cmp x0, x0
    cset x2, eq
    ASSERT_EQ(x2, 1)

    /* eq is false, set x2 = 0. */
    cmp x0, x1
    cset x2, eq
    ASSERT_EQ(x2, 0)

    /* Same for ne. */
    cmp x0, x0
    cset x2, ne
    ASSERT_EQ(x2, 0)

    cmp x0, x1
    cset x2, ne
    ASSERT_EQ(x2, 1)
EXIT
