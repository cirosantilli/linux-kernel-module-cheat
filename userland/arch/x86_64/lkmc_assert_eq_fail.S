/* https://github.com/cirosantilli/linux-kernel-module-cheat#userland-assembly */

#include <lkmc.h>

LKMC_PROLOGUE
    mov $0x123456789ABCDEF0, %r12
    mov $0x123456789ABCDEF0, %r13
    mov $0x111111119ABCDEF0, %r14

    /* Passing examples. */

    /* Register immediate. */
    LKMC_ASSERT_EQ(%r12, $0x123456789ABCDEF0)
    LKMC_ASSERT_EQ($0x123456789ABCDEF0, %r12)

    /* Register register. */
    LKMC_ASSERT_EQ(%r12, %r13)
    LKMC_ASSERT_EQ(%r13, %r12)

    /* Register memory. */
    LKMC_ASSERT_EQ(%r12, myvar)
    LKMC_ASSERT_EQ(myvar, %r12)

    /* 32-bit versions. */
    LKMC_ASSERT_EQ_32(%r12d, %r14d)
    LKMC_ASSERT_EQ_32(%r12d, myvar2)

    /* Now let's fail. */
    LKMC_ASSERT_EQ(%r12, $0x123456789ABCDEF1)
LKMC_EPILOGUE
myvar:  .quad 0x123456789ABCDEF0
myvar2: .quad 0x111111119ABCDEF0
