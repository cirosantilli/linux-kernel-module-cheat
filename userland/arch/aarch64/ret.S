/* https://github.com/cirosantilli/arm-assembly-cheat#bl */

#include "common.h"

ENTRY
    mov x0, 1
    bl inc
    ASSERT_EQ(x0, 2)
    bl inc2
    ASSERT_EQ(x0, 3)
    bl inc3
    ASSERT_EQ(x0, 4)
EXIT

/* void inc(uint64_t *i) { (*i)++ } */
inc:
    add x0, x0, 1
    ret

/* Same but explicit return register. */
inc2:
    add x0, x0, 1
    ret x30

/* Same but with br. */
inc3:
    add x0, x0, 1
    br x30
