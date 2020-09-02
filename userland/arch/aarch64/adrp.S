/* https://cirosantilli.com/linux-kernel-module-cheat#arm-adr-instruction */

#include <lkmc.h>

LKMC_PROLOGUE
    /* Actual usage pattern. */
    adrp x19, mylong
    add x19, x19, :lo12:mylong
    ldr x20, [x19]
    LKMC_ASSERT_EQ(x19, =mylong)
    LKMC_ASSERT_EQ(x20, =0x1122334455667788)

    /* Minimal testcase. */
    adrp x0, .Llabel
    adr x1, .Llabel
.Llabel:
    /* Clear the lower 12 bits. */
    bic x1, x1, 0xFF
    bic x1, x1, 0xF00
    LKMC_ASSERT_EQ_REG(x0, x1)
LKMC_EPILOGUE
.data
mylong:
   .quad 0x1122334455667788
