/* https://cirosantilli.com/linux-kernel-module-cheat#arm-ldrh-and-ldrb-instructions */

#include <lkmc.h>

LKMC_PROLOGUE
    ldr x19, =zero_extend

    mov x0, 0x0
    ldr w0, [x19]
    LKMC_ASSERT_EQ(x0, =0x0000000012345678)

    mov x0, 0x0
    ldrsw x0, [x19]
    LKMC_ASSERT_EQ(x0, =0x0000000012345678)

    ldr x19, =one_extend

    mov x0, 0x0
    ldr w0, [x19]
    LKMC_ASSERT_EQ(x0, =0x0000000082345678)

    mov x0, 0x0
    ldrsw x0, [x19]
    LKMC_ASSERT_EQ(x0, =0xFFFFFFFF82345678)

LKMC_EPILOGUE
zero_extend:
    .word 0x12345678
one_extend:
    .word 0x82345678
