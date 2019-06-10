/* https://github.com/cirosantilli/linux-kernel-module-cheat#userland-assembly */

#include <lkmc.h>

LKMC_PROLOGUE
    /* Register immediate. */
    mov $1, %rax
    add $2, %rax
    LKMC_ASSERT_EQ(%rax, $3)

    /* Register register. */
    mov $1, %rax
    mov $2, %rbx
    add %rbx, %rax
    LKMC_ASSERT_EQ(%rax, $3)

    /* Memory and immediate. */
.data
    memory_immediate: .quad 0x123456789ABCDEF0
.text
    addq $1, memory_immediate
    LKMC_ASSERT_EQ(memory_immediate, $0x123456789ABCDEF1)

    /* Memory and register. */
.data
    memory_register: .quad 0x123456789ABCDEF0
.text
    mov $1, %rax
    add %rax, memory_register
    LKMC_ASSERT_EQ(memory_register, $0x123456789ABCDEF1)

#if 0
    /* Cannot do different register sizes:
     * Error: operand type mismatch for for `add'
     */
    add %rax, %eax
#endif

#if 0
    /* Cannot do memory memory.
     * Error: too many memory references for `add'
     */
    addq memory_immediate, memory_register
#endif
LKMC_EPILOGUE
