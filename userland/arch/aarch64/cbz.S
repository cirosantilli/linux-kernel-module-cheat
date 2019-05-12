/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-cbz-instruction */

#include "common.h"

ENTRY

    /* Branch. */
    mov x0, 0x0
    cbz x0, ok
    FAIL
ok:

    /* Don't branch. */
    mov x0, 0x1
    cbz x0, ko

EXIT
ko:
    FAIL
