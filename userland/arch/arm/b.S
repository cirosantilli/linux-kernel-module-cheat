/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-b-instruction */

#include "common.h"
ENTRY
    /* Jump over the fail. 26-bit PC-relative. */
    b ok
    FAIL
ok:
EXIT
