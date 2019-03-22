/* https://github.com/cirosantilli/arm-assembly-cheat#data-processing-instructions */

#include "common.h"

ENTRY
    ldr r0, =0x7FFFFFFF
    clz r1, r0
    ASSERT_EQ(r1, 1)

    ldr r0, =0x3FFFFFFF
    clz r1, r0
    ASSERT_EQ(r1, 2)

    ldr r0, =0x1FFFFFFF
    clz r1, r0
    ASSERT_EQ(r1, 3)
EXIT
