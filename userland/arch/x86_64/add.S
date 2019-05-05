/* https://github.com/cirosantilli/linux-kernel-module-cheat#userland-assembly */

#include "common.h"

ENTRY
    mov $1, %rax
    add $2, %rax
    ASSERT_EQ(%rax, $3)
EXIT
