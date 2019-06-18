#ifndef LKMC_X86_64_H
#define LKMC_X86_64_H

/* This and other macros may make C function calls, and therefore can destroy
 * non-callee saved registers. */
#define LKMC_ASSERT_EQ(general1, general2) \
    push %rdi; \
    mov general2, %rdi; \
    push %rdi; \
    mov 8(%rsp), %rdi; \
    mov general1, %rdi; \
    pop %rsi; \
    add $8, %rsp; \
    mov $__LINE__, %edx; \
    call lkmc_assert_eq_64; \
;

#define LKMC_ASSERT_EQ_32(general1, general2) \
    push %rdi; \
    mov general2, %edi; \
    push %rdi; \
    mov 8(%rsp), %rdi; \
    mov general1, %edi; \
    pop %rsi; \
    add $8, %rsp; \
    mov $__LINE__, %edx; \
    call lkmc_assert_eq_32; \
;

#define LKMC_ASSERT_FAIL \
    mov $__LINE__, %edi; \
    call lkmc_assert_fail; \
;

/* Assert that two memory arrays are the same. */
#define LKMC_ASSERT_MEMCMP(label1, label2, const_size) \
    lea label1(%rip), %rdi; \
    lea label2(%rip), %rsi; \
    mov const_size, %rdx; \
    mov $__LINE__, %ecx; \
    call lkmc_assert_memcmp; \
;

/* Function epilogue.
 *
 * https://github.com/cirosantilli/linux-kernel-module-cheat#x86_64-calling-convention
 */
#define LKMC_EPILOGUE \
    add $8, %rsp; \
    pop %rbx; \
    pop %r12; \
    pop %r13; \
    pop %r14; \
    pop %r15; \
    pop %rbp; \
    mov $0, %rax; \
    ret; \
;

/* Function prologue.
 *
 * https://github.com/cirosantilli/linux-kernel-module-cheat#x86_64-calling-convention
 */
#define LKMC_PROLOGUE \
.text; \
.global main; \
main: \
    push %rbp; \
    mov %rsp, %rbp; \
    push %r15; \
    push %r14; \
    push %r13; \
    push %r12; \
    push %rbx; \
    sub $8, %rsp; \
main_after_prologue: \
;

#endif
