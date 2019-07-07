/* https://cirosantilli.com/linux-kernel-module-cheat#arm-adr-instruction */

#include <lkmc.h>

LKMC_PROLOGUE
    adrp x0, .Llabel
    adr x1, .Llabel
.Llabel:
    /* Clear the lower 12 bits. */
    bic x1, x1, 0xFF
    bic x1, x1, 0xF00
    LKMC_ASSERT_EQ_REG(x0, x1)
LKMC_EPILOGUE
