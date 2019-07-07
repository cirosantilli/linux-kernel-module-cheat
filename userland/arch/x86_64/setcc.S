/* https://cirosantilli.com/linux-kernel-module-cheat#x86-bit-and-byte-instructions */

#include <lkmc.h>

LKMC_PROLOGUE
    mov $0xFF, %r12

    /* Set Carry flag. */
    stc
    /* Check for carry flag. */
    setc %r12b
    /* Carry flag was set, so set the r12b to 1. */
    LKMC_ASSERT_EQ(%r12, $1)

    /* Clear carry flag. */
    clc
    setc %r12b
    LKMC_ASSERT_EQ(%r12, $0)

#if 0
    /* The operand size can only be one byte:
     * Error: operand size mismatch for `setc'
     */
    setc %eax
#endif
LKMC_EPILOGUE
