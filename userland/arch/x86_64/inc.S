/* Increment: i++.
 *
 * https://github.com/cirosantilli/linux-kernel-module-cheat#x86-binary-arithmetic-instructions
 */

#include <lkmc.h>

LKMC_PROLOGUE
    mov $2, %eax
    inc %eax
    LKMC_ASSERT_EQ_32(%eax, $3)
LKMC_EPILOGUE
