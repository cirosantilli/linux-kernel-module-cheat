/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-bic-instruction */

#include "common.h"

ENTRY
    /* 0x0F & ~0x55 == 0x0F & 0xAA == 0x0A */
    mov r0, 0x0F
    bic r0, 0x55
    ASSERT_EQ(r0, 0x0A)
EXIT
