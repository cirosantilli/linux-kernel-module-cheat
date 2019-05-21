/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-cbz-instruction */

#include "common.h"

LKMC_ENTRY

    /* Branch. */
    mov x0, 0x0
    cbz x0, ok
    LKMC_FAIL
ok:

    /* Don't branch. */
    mov x0, 0x1
    cbz x0, ko

LKMC_EXIT
ko:
    LKMC_FAIL
