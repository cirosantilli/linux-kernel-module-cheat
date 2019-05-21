/* https://github.com/cirosantilli/linux-kernel-module-cheat#data-bitwise-instructions */

#include "common.h"

LKMC_ENTRY
    ldr r0, =0x7FFFFFFF
    clz r1, r0
    LKMC_ASSERT_EQ(r1, 1)

    ldr r0, =0x3FFFFFFF
    clz r1, r0
    LKMC_ASSERT_EQ(r1, 2)

    ldr r0, =0x1FFFFFFF
    clz r1, r0
    LKMC_ASSERT_EQ(r1, 3)
LKMC_EXIT
