/* https://github.com/cirosantilli/linux-kernel-module-cheat#userland-assembly */

#include "common.h"

ENTRY
    mov $1, %rax
    ASSERT_EQ(%rax, $1)
    ASSERT_EQ(%rax, $2)
EXIT
