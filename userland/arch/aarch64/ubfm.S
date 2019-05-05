/* https://github.com/cirosantilli/arm-assembly-cheat#ubfm */

#include "common.h"

ENTRY
    ldr x19, =0x1122334455667788

    // lsr alias: imms == 63

    ldr x20, =0xFFFFFFFFFFFFFFFF
    ubfm x20, x19, 16, 63
    ASSERT_EQ(x20, 0x0000112233445566)

    ldr x20, =0xFFFFFFFFFFFFFFFF
    ubfm x20, x19, 32, 63
    ASSERT_EQ(x20, 0x0000000011223344)
EXIT
