/* https://github.com/cirosantilli/arm-assembly-cheat#ubfm */

#include "common.h"

ENTRY
    ldr x0, =0x1122334455667788

    // lsr alias: imms == 63

    ldr x1, =0xFFFFFFFFFFFFFFFF
    ubfm x1, x0, 16, 63
    ASSERT_EQ(x1, 0x0000112233445566)

    ldr x1, =0xFFFFFFFFFFFFFFFF
    ubfm x1, x0, 32, 63
    ASSERT_EQ(x1, 0x0000000011223344)
EXIT
