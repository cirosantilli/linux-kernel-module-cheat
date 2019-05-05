/* https://github.com/cirosantilli/arm-assembly-cheat#ubfx */

#include "common.h"

ENTRY
    ldr x0, =0x1122334455667788

    ldr x1, =0xFFFFFFFFFFFFFFFF
    ubfx x1, x0, 8, 16
    ASSERT_EQ(x1, 0x0000000000006677)

    ldr x1, =0xFFFFFFFFFFFFFFFF
    ubfx x1, x0, 8, 32
    ASSERT_EQ(x1, 0x0000000044556677)
EXIT
