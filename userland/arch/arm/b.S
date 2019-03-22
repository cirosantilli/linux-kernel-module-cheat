/* https://github.com/cirosantilli/arm-assembly-cheat#b */

#include "common.h"
ENTRY
    /* Jump over the fail. 26-bit PC-relative. */
    b ok
    FAIL
ok:
EXIT
