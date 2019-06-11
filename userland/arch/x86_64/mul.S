/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-binary-arithmetic-instructions
 *
 * Unsigned multiply.
 *
 * The result is spread across edx:eax.
 */

#include <lkmc.h>

LKMC_PROLOGUE

    /* 64-bit hello world:
     *
     * rdx : rax = rax * rbx
     * 0x0 : 4   =   2 *   2
     */
    mov $2, %rax
    mov $2, %rbx
    mul %rbx
    /* Move to callee saved registers to persist after our asserts. */
    mov %rax, %r12
    mov %rdx, %r13
    mov %rbx, %r14
    LKMC_ASSERT_EQ(%r12, $4)
    LKMC_ASSERT_EQ(%r13, $0)
    /* rbx is untouched. */
    LKMC_ASSERT_EQ(%r14, $2)

    /* 64-bit with a carry:
     *
     * rdx :                rax = rax                * rbx
     * 0x1 : 0x0000000000000002 = 0x8000000000000001 *   2
     */
    mov $0x8000000000000001, %rax
    mov $2, %rbx
    mul %rbx
    mov %rax, %r12
    mov %rdx, %r13
    LKMC_ASSERT_EQ(%r12, $2)
    LKMC_ASSERT_EQ(%r13, $1)

    /* 8-bit is special: does not use dx for output:
     *
     *   ah : al = al *   bl
     * 0x10 :  0 =  2 * 0x80
     */
    mov $0, %eax
    mov $2, %al
    mov $0x80, %bl
    mov $0, %dl
    mul %bl
    LKMC_ASSERT_EQ_32(%eax, $0x100)

    /* 16-bit
     *
     *  dx :     ax = ax *     bx
     * 0x1 : 0x0000 =  2 * 0x8000
     */
    mov $0, %eax
    mov $0, %edx
    mov $2, %ax
    mov $0x8000, %bx
    mov $0, %dx
    mul %bx
    mov %eax, %r12d
    mov %edx, %r13d
    LKMC_ASSERT_EQ_32(%r12d, $0)
    LKMC_ASSERT_EQ_32(%r13d, $1)

    /* 32-bit */
    mov $2, %eax
    mov $0x80000000, %ebx
    mov $0, %edx
    mul %ebx
    mov %eax, %r12d
    mov %edx, %r13d
    LKMC_ASSERT_EQ_32(%r12d, $0)
    LKMC_ASSERT_EQ_32(%r13d, $1)


#if 0
    /* No immediate form, although imul has one:
     * http://stackoverflow.com/questions/20499141/is-it-possible-to-multiply-by-and-immediate-with-mul-in-x86-assembly/33202309#33202309
     *
     * Error: operand type mismatch for `mul'
     */
    mul $2
#endif

    /* Memory version */
.data
    mylong: .long 0x11111111
.text
    movl $2, %eax
    mull mylong
    LKMC_ASSERT_EQ_32(%eax, $0x22222222)

LKMC_EPILOGUE
