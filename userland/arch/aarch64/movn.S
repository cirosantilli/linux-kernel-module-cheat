/* https://github.com/cirosantilli/arm-assembly-cheat#movn */

#include "common.h"

ENTRY
    ldr x0, =0x123456789ABCDEF0
    movn x0, 0x8888, lsl 16
    ASSERT_EQ(x0, 0xFFFFFFFF7777FFFF)
EXIT
