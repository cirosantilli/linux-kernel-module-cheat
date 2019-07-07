/* https://cirosantilli.com/linux-kernel-module-cheat#x86-shift-and-rotate-instructions */

#include <lkmc.h>

LKMC_PROLOGUE
    mov $0x81, %r12

    /* Shift left by one. */
    shl %r12b
    LKMC_ASSERT(jc)
    LKMC_ASSERT_EQ(%r12, $2)

    /* Shift left by one. */
    shl %r12b
    LKMC_ASSERT(jnc)
    LKMC_ASSERT_EQ(%r12, $4)

    /* Shift right by one. */
    shr %r12b
    LKMC_ASSERT(jnc)
    LKMC_ASSERT_EQ(%r12, $2)

    /* Shift left by 2 immediate.
     * Differentent coding than shift by 1.
     */
    shl $2, %r12b
    LKMC_ASSERT(jnc)
    LKMC_ASSERT_EQ(%r12, $8)

    /* Shift left by 2 in cl register. */
    mov $2, %cl
    shl %cl, %r12b
    LKMC_ASSERT(jnc)
    LKMC_ASSERT_EQ(%r12, $0x20)

#if 0
    /* cl is the only possible register choice
     * Error: operand type mismatch for `shr'
     */
    shr %bl, %ax
#endif

LKMC_EPILOGUE
