/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-cmovcc-instructions */

#include <lkmc.h>

LKMC_PROLOGUE
    /* Carry flag clear. */
    clc
    mov $0, %rax
    mov $1, %rbx
    /* Don't move: carry flag not set. */
    cmovc %rbx, %rax
    LKMC_ASSERT_EQ(%rax, $0)

    /* Carry flag clear. */
    clc
    mov $0, %rax
    mov $1, %rbx
    /* Move because checking NC. */
    cmovnc %rbx, %rax
    LKMC_ASSERT_EQ(%rax, $1)

    /* Carry flag set. */
    stc
    mov $0, %rax
    mov $1, %rbx
    /* Move. */
    cmovc %rbx, %rax
    LKMC_ASSERT_EQ(%rax, $1)

#if 0
    /* Immediates not supported:
     * Error: operand type mismatch for `cmovc' */
    cmovc $1, %rax
#endif
LKMC_EPILOGUE
