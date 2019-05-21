/* https://github.com/cirosantilli/linux-kernel-module-cheat#userland-assembly */

#include "common.h"

LKMC_ENTRY
    mov $0x123456789ABCDEF0, %r12
    mov $0x123456789ABCDEF0, %r13
    LKMC_ASSERT_EQ(%r12, $0x123456789ABCDEF0)
    LKMC_ASSERT_EQ(%r12, %r13)
    LKMC_ASSERT_EQ(%r12, myvar)
    LKMC_ASSERT_EQ($0x123456789ABCDEF0, %r12)
    LKMC_ASSERT_EQ(%r13, %r12)
    LKMC_ASSERT_EQ(myvar, %r12)
    LKMC_ASSERT_EQ(%r12, $0x123456789ABCDEF1)
LKMC_EXIT
myvar: .quad 0x123456789ABCDEF0
