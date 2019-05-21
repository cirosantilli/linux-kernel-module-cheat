/* https://github.com/cirosantilli/linux-kernel-module-cheat#armv8-aarch64-ret-instruction */

#include "common.h"

LKMC_ENTRY
    mov x19, 1
    bl inc
    LKMC_ASSERT_EQ(x19, 2)
    bl inc2
    LKMC_ASSERT_EQ(x19, 3)
    bl inc3
    LKMC_ASSERT_EQ(x19, 4)
LKMC_EXIT

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
