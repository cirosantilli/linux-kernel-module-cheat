/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-adr-instruction */

#include "common.h"

LKMC_ENTRY
    adrp x0, label
    adr x1, label
label:
    /* Clear the lower 12 bits. */
    bic x1, x1, 0xFF
    bic x1, x1, 0xF00
    LKMC_ASSERT_EQ_REG(x0, x1)
LKMC_EXIT
