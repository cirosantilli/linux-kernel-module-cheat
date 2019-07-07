/* https://cirosantilli.com/linux-kernel-module-cheat#x86-registers */

#include <lkmc.h>

LKMC_PROLOGUE
    /* All the 16 general purpose registers. */
    mov $0x0, %rax
    mov $0x0, %rcx
    mov $0x0, %rdx
    mov $0x0, %rbx
#if 0
    /* We don't want to do touch those because if affets our main call stack. */
    mov $0x0, %rbp
    mov $0x0, %rsp
#endif
    mov $0x0, %rsi
    mov $0x0, %rdi
    mov $0x0, %r8
    mov $0x0, %r9
    mov $0x0, %r10
    mov $0x0, %r11
    mov $0x0, %r12
    mov $0x0, %r13
    mov $0x0, %r14
    mov $0x0, %r15

    /* EAX and AX. */
    mov $0x11111111, %eax
    mov $0x2222, %ax
    LKMC_ASSERT_EQ_32(%eax, $0x11112222)

    /* EAX, AL and AH. */
    mov $0x11111111, %eax
    mov $0x22, %ah
    mov $0x33, %al
    LKMC_ASSERT_EQ_32(%eax, $0x11112233)

    /* R12D, R12W and R12B */
    mov $0x11111111, %r12d
    mov $0x2222, %r12w
    mov $0x33, %r12b
    LKMC_ASSERT_EQ_32(%r12d, $0x11112233)

#if 0
    /* There is no R12H: the second byte is only addressable on the old registers.
     * Error: bad register name `%r12h' */
    mov $0x22, %r12h
#endif

    /* Keep in mind that most 32-bit register instructions zero out the top bits of RAX! 
     * https://stackoverflow.com/questions/11177137/why-do-x86-64-instructions-on-32-bit-registers-zero-the-upper-part-of-the-full-6 */
    mov $0x1122334455667788, %r12
    LKMC_ASSERT_EQ_32(%r12d, $0x55667788)
    mov $0x99AABBCC, %r12
    LKMC_ASSERT_EQ(%r12, $0x0000000099AABBCC)
LKMC_EPILOGUE
