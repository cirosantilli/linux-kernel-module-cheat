/* https://github.com/cirosantilli/arm-assembly-cheat#bl */

#include "common.h"

ENTRY
    mov x19, 1
    bl inc
    ASSERT_EQ(x19, 2)
    bl inc2
    ASSERT_EQ(x19, 3)
    bl inc3
    ASSERT_EQ(x19, 4)
EXIT

/* void inc(uint64_t *i) { (*i)++ } */
inc:
    add x19, x19, 1
    ret

/* Same but explicit return register. */
inc2:
    add x19, x19, 1
    ret x30

/* Same but with br. */
inc3:
    add x19, x19, 1
    br x30
