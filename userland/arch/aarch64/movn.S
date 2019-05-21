/* https://github.com/cirosantilli/linux-kernel-module-cheat#armv8-aarch64-movn-instruction */

#include "common.h"

LKMC_ENTRY
    ldr x0, =0x123456789ABCDEF0
    movn x0, 0x8888, lsl 16
    LKMC_ASSERT_EQ(x0, 0xFFFFFFFF7777FFFF)
LKMC_EXIT
