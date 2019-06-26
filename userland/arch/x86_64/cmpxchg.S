/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-cmpxchg-instruction */

#include <lkmc.h>

LKMC_PROLOGUE
    /* rax != r13 */
    mov $0, %rax
    mov $1, %r13
    mov $2, %r14
    cmpxchg %r14, %r13
    mov %rax, %r12
    LKMC_ASSERT(jnz)
    LKMC_ASSERT_EQ(%rax, $1)
    LKMC_ASSERT_EQ(%r13, $1)
    LKMC_ASSERT_EQ(%r14, $2)

    /* rax == r13 */
    mov $0, %rax
    mov $0, %r13
    mov $2, %r14
    cmpxchg %r14, %r13
    mov %rax, %r12
    LKMC_ASSERT(jz)
    LKMC_ASSERT_EQ(%rax, $0)
    LKMC_ASSERT_EQ(%r13, $2)
    LKMC_ASSERT_EQ(%r14, $2)
LKMC_EPILOGUE
