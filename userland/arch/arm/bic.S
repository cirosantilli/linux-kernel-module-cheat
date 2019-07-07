/* https://cirosantilli.com/linux-kernel-module-cheat#arm-bic-instruction */

#include <lkmc.h>

LKMC_PROLOGUE
    /* 0x0F & ~0x55 == 0x0F & 0xAA == 0x0A */
    mov r0, 0x0F
    bic r0, 0x55
    LKMC_ASSERT_EQ(r0, =0x0A)
LKMC_EPILOGUE
