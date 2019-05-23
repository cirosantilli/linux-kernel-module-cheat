/* https://github.com/cirosantilli/linux-kernel-module-cheat#userland-assembly */

#include <lkmc.h>

LKMC_PROLOGUE
    ldr x19, =0x123456789ABCDEF0
    ldr x20, =0x123456789ABCDEF0
    ldr x21, =0x000000009ABCDEF0

    /* Passing examples. */

    /* Register immediate. */
    LKMC_ASSERT_EQ(x19, =0x123456789ABCDEF0)
#if 0
    /* Syntax not supported. */
    LKMC_ASSERT_EQ(=0x123456789ABCDEF0, x19)
#endif

    /* Register register. */
    LKMC_ASSERT_EQ_REG(x19, x20)
    LKMC_ASSERT_EQ_REG(x20, x19)
    LKMC_ASSERT_EQ_REG_32(w19, w21)
    LKMC_ASSERT_EQ_REG_32(w21, w19)

    /* Register memory. */
    LKMC_ASSERT_EQ(x19, myvar)
#if 0
    /* Syntax not supported. */
    LKMC_ASSERT_EQ(myvar, x19)
#endif

    /* Now let's fail. */
    LKMC_ASSERT_EQ(x19, =0x123456789ABCDEF1)
LKMC_EPILOGUE
myvar: .quad 0x123456789ABCDEF0
