/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-shift-and-rotate-instructions */

#include <lkmc.h>

LKMC_PROLOGUE
    /* 0xFF == -1 in 2's complement with 8-bits. */
    mov $0xFF, %r12
    sal %r12b
    LKMC_ASSERT(jc)
    /* 0xFE == -2 in 2's complement with 8-bits. */
    LKMC_ASSERT_EQ(%r12, $0xFE)

    /* SAR*/
    sar %r12b
    LKMC_ASSERT(jnc)
    /* -1 */
    LKMC_ASSERT_EQ(%r12, $0xFF)

    /* SAR rounds to -infinity: -1 goes to -1 again. */
    sar %r12b
    LKMC_ASSERT(jc)
    /* -1 */
    LKMC_ASSERT_EQ(%r12, $0xFF)
LKMC_EPILOGUE
