/* https://github.com/cirosantilli/arm-assembly-cheat#rbit */

#include "common.h"

ENTRY
    ldr r0,      =0b00000001001000110100010101100101
    rbit r1, r0
    ASSERT_EQ(r1, 0b10100110101000101100010010000000)
EXIT
