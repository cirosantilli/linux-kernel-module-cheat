/* https://github.com/cirosantilli/linux-kernel-module-cheat#userland-assembly */

#include "common.h"

LKMC_ENTRY
    mov x0, 1
    add x1, x0, 2
    LKMC_ASSERT_EQ(x1, 3)
LKMC_EXIT
