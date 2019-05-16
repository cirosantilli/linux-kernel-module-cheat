/* https://github.com/cirosantilli/linux-kernel-module-cheat#userland-assembly */

#include "common.h"

ENTRY
    mov $0x123456789ABCDEF0, %r12
    mov $0x123456789ABCDEF0, %r13
    ASSERT_EQ(%r12, $0x123456789ABCDEF0)
    ASSERT_EQ(%r12, %r13)
    ASSERT_EQ(%r12, myvar)
    ASSERT_EQ($0x123456789ABCDEF0, %r12)
    ASSERT_EQ(%r13, %r12)
    ASSERT_EQ(myvar, %r12)
    ASSERT_EQ(%r12, $0x123456789ABCDEF1)
EXIT
myvar: .quad 0x123456789ABCDEF0
