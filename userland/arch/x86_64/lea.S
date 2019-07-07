/* https://cirosantilli.com/linux-kernel-module-cheat#x86-data-transfer-instructions
 *
 * Load Effective Address.
 *
 * Like MOV, but load the address instead of the value.
 *
 * Useful in particular for RIP relative addressing.
 */

#include <lkmc.h>

LKMC_PROLOGUE
    /* RIP relative addressing. */
    lea my_label(%rip), %rax
    LKMC_ASSERT_EQ(%rax, $my_label)

    /* Also supports the usual addressing operations. */
    mov $my_label, %rax
    mov $2, %rbx
    lea 4(%rax,%rbx,2), %rdx
    LKMC_ASSERT_EQ(%rdx, $my_label_2)
LKMC_EPILOGUE
my_label: .skip 8
my_label_2:
