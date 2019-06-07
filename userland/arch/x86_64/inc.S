/* Increment: i++.
 *
 * https://github.com/cirosantilli/linux-kernel-module-cheat#x86-binary-arithmetic-instructions
 */

#include <lkmc.h>

LKMC_PROLOGUE
    mov $2, %rax
    inc %rax
    LKMC_ASSERT_EQ(%rax, $3)
LKMC_EPILOGUE
