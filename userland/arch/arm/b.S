/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-b-instruction */

#include "common.h"
LKMC_ENTRY
    /* Jump over the fail. 26-bit PC-relative. */
    b ok
    LKMC_FAIL
ok:
LKMC_EXIT
