/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-binary-arithmetic-instructions
 *
 * Signed multiply.
 *
 * Has many more forms than MUL including immediate and up to three arguments.
 */

#include <lkmc.h>

LKMC_PROLOGUE

    /* The single register forms are just like MUL, and
     * extend over rdx:rax.
     *
     * rdx : rax =  rax * rbx
     *           =   -1 *   2
     *           =  -2
     *           =  0xFFFFFFFFFFFFFFFF : 0xFFFFFFFFFFFFFFFE
     */
    mov $-1, %rax
    mov $2, %rbx
    mov $42, %rdx
    imul %rbx
    mov %rax, %r12
    mov %rdx, %r13
    LKMC_ASSERT_EQ(%r12, $0xFFFFFFFFFFFFFFFE)
    LKMC_ASSERT_EQ(%r13, $0xFFFFFFFFFFFFFFFF)

    /* The multi-argument formas don't extend over rdx, but
     * are more convenient in many cases.
     *
     * rax = rbx * 3
     */
    mov $42, %rax
    mov $-2, %rbx
    mov $42, %rdx
    imul $3, %rbx, %rax
    mov %rax, %r12
    mov %rdx, %r13
    LKMC_ASSERT_EQ(%r12, $-6)
    LKMC_ASSERT_EQ(%r13, $42)
LKMC_EPILOGUE
