/* https://cirosantilli.com/linux-kernel-module-cheat#x86-control-transfer-instructions
 *
 * Unconditional branch to an absolute address stored in memory on in a register.
 */

#include <lkmc.h>

LKMC_PROLOGUE
    /* Address in memory. */
.section .rodata
    label_address: .quad .Lmemory_label
.text
    jmp *label_address
    LKMC_ASSERT_FAIL
.Lmemory_label:

    /* Address in register. */
    lea .Lregister_label(%rip), %rax
    jmp *%rax
    LKMC_ASSERT_FAIL
.Lregister_label:
LKMC_EPILOGUE
