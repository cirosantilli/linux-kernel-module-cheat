/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-binary-arithmetic-instructions
 *
 * Unsigned integer division, interface similar to MUL:
 *
 * ....
 * rax = rdx:rax / SRC
 * rdx = rdx:rax % SRC
 * ....
 *
 * DIV can be used to calculate modulus, but GCC does not use it becaues it is slow, 
 * and choses alternative techniques instead
 * http://stackoverflow.com/questions/4361979/how-does-the-gcc-implementation-of-module-work-and-why-does-it-not-use-the
 */

#include <lkmc.h>

LKMC_PROLOGUE
    /* 64-bit hello world:
     *
     * 5 / 2 = 2 with leftover of 1.
     */
    mov $0, %rdx
    mov $5, %rax
    mov $2, %rbx
    div %rbx
    mov %rax, %r12
    mov %rdx, %r13
    LKMC_ASSERT_EQ(%r12, $2)
    LKMC_ASSERT_EQ(%r13, $1)

    /* Now with a simple carry. */
    mov $1, %rdx
    mov $2, %rax
    mov $2, %rbx
    div %rbx
    mov %rax, %r12
    mov %rdx, %r13
    LKMC_ASSERT_EQ(%r12, $0x8000000000000001)
    LKMC_ASSERT_EQ(%r13, $0)
LKMC_EPILOGUE
