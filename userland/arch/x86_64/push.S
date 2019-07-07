/* https://cirosantilli.com/linux-kernel-module-cheat#x86-push-and-pop-instructions */

#include <lkmc.h>

LKMC_PROLOGUE
    /* register hello world. */
    mov %rsp, %r12
    mov $0x123456789ABCDEF0, %rax
    push %rax
    /* Save the stack delta. */
    sub %rsp, %r12
    /* Save the stack value. */
    mov (%rsp), %r13
    /* Restore the stack and save its value to R14. */
    pop %r14
    /* The stack still goes down by 8 even though we pushed a 4-byte immediate. */
    LKMC_ASSERT_EQ(%r12, $8)
    LKMC_ASSERT_EQ(%r13, $0x123456789ABCDEF0)
    LKMC_ASSERT_EQ(%r14, $0x123456789ABCDEF0)

    /* Immediate. Can only push up to 4 byte immediates. */
    mov %rsp, %r12
    push $0x12345678
    sub %rsp, %r12
    mov (%rsp), %r13
    pop %r14
    /* The stack still goes down by 8 even though we pushed a 4-byte immediate. */
    LKMC_ASSERT_EQ(%r12, $8)
    LKMC_ASSERT_EQ(%r13, $0x12345678)
    LKMC_ASSERT_EQ(%r14, $0x12345678)

    /* Word example. */
    mov %rsp, %r12
    mov $0x1234, %ax
    push %ax
    sub %rsp, %r12
    mov $0, %r13
    mov (%rsp), %r13w
    mov $0, %r14
    pop %r14w
    /* The stack was decremented only by 2 as expected. */
    LKMC_ASSERT_EQ(%r12, $2)
    LKMC_ASSERT_EQ(%r13, $0x1234)
    LKMC_ASSERT_EQ(%r14, $0x1234)
LKMC_EPILOGUE
