/* https://github.com/cirosantilli/linux-kernel-module-cheat#userland-assembly */

#include <lkmc.h>

LKMC_PROLOGUE
    ldr r4, =0x12345678
    ldr r5, =0x12345678

    /* Passing examples. */

    /* Register immediate. */
    LKMC_ASSERT_EQ(r4, =0x12345678)
#if 0
    /* Syntax not supported. */
    LKMC_ASSERT_EQ(=0x12345678, r4)
#endif

    /* Register register. */
    LKMC_ASSERT_EQ_REG(r4, r5)
    LKMC_ASSERT_EQ_REG(r5, r4)

    /* Register memory. */
    LKMC_ASSERT_EQ(r4, myvar)
#if 0
    /* Syntax not supported. */
    LKMC_ASSERT_EQ(myvar, r4)
#endif

    /* Now let's fail. */
    LKMC_ASSERT_EQ(r4, =0x12345679)
LKMC_EPILOGUE
myvar: .quad 0x12345678
