/* https://github.com/cirosantilli/arm-assembly-cheat#bfi */

#include "common.h"

ENTRY
    ldr r0, =0x11223344
    ldr r1, =0xFFFFFFFF
    bfi r1, r0, 8, 16
    ASSERT_EQ(r1, 0xFF3344FF)
EXIT
