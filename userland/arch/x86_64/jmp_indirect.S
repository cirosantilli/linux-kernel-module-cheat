/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-control-transfer-instructions
 *
 * Unconditional branch to an absolute address stored in memory on in a register.
 */

#include <lkmc.h>

LKMC_PROLOGUE
    /* Address in memory. */
.section .rodata
    label_address: .quad memory_label
.text
    jmp *label_address
    LKMC_ASSERT_FAIL
memory_label:

    /* Address in register. */
    lea register_label(%rip), %rax
    jmp *%rax
    LKMC_ASSERT_FAIL
register_label:
LKMC_EPILOGUE
