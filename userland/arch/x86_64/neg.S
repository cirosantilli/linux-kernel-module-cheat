/* https://cirosantilli.com/linux-kernel-module-cheat#x86-binary-arithmetic-instructions
 *
 * Negate: i *= -1.
 */

#include <lkmc.h>

LKMC_PROLOGUE
    mov $2, %rax
    neg %rax
    LKMC_ASSERT_EQ(%rax, $-2)
    neg %eax
    LKMC_ASSERT_EQ(%rax, $2)
LKMC_EPILOGUE
