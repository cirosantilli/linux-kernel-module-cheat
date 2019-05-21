/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-ldrh-instruction-and-ldrb */

#include "common.h"

LKMC_ENTRY
    ldr r0, =myvar
    mov r1, 0x0
    ldrh r1, [r0]
    LKMC_ASSERT_EQ(r1, 0x00005678)
LKMC_EXIT
myvar:
    .word 0x12345678
