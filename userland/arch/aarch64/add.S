/* https://github.com/cirosantilli/arm-assembly-cheat#data-processing-instructions */

#include "common.h"

ENTRY
    mov x0, 1
    add x1, x0, 2
    ASSERT_EQ(x1, 3)
EXIT
