/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-ubfx-instruction */

#include "common.h"

LKMC_ENTRY
    ldr x19, =0x1122334455667788

    ldr x20, =0xFFFFFFFFFFFFFFFF
    ubfx x20, x19, 8, 16
    LKMC_ASSERT_EQ(x20, 0x0000000000006677)

    ldr x20, =0xFFFFFFFFFFFFFFFF
    ubfx x20, x19, 8, 32
    LKMC_ASSERT_EQ(x20, 0x0000000044556677)
LKMC_EXIT
