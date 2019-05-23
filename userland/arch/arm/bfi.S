/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-bfi-instruction */

#include <lkmc.h>

LKMC_PROLOGUE
    ldr r0, =0x11223344
    ldr r1, =0xFFFFFFFF
    bfi r1, r0, 8, 16
    LKMC_ASSERT_EQ(r1, =0xFF3344FF)
LKMC_EPILOGUE
