/* https://github.com/cirosantilli/arm-assembly-cheat#armv8-str */

#include "common.h"

ENTRY
    ldr x0, myvar
    ASSERT_EQ(x0, 0x12346789ABCDEF0)
#if 0
    /* Error: invalid addressing mode at operand 2 -- `str x0,myvar' */
    str x0, myvar
#endif
EXIT
    myvar: .quad 0x12346789ABCDEF0
