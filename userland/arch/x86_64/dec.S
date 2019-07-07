/* Decrement: i--.
 *
 * https://cirosantilli.com/linux-kernel-module-cheat#x86-binary-arithmetic-instructions
 */

#include <lkmc.h>

LKMC_PROLOGUE
    mov $3, %rax
    dec %rax
    LKMC_ASSERT_EQ(%rax, $2)
LKMC_EPILOGUE
