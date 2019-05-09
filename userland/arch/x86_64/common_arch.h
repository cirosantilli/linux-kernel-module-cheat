/* https://github.com/cirosantilli/linux-kernel-module-cheat#userland-assembly-c-standard-library */

#ifndef COMMON_ARCH_H
#define COMMON_ARCH_H

#define ASSERT_EQ(general1, general2) \
    mov general2, %rdi; \
    push %rdi; \
    mov general1, %rdi; \
    pop %rsi; \
    ASSERT_EQ_DO(64); \
;

#define ASSERT_EQ_DO(bits) \
    call assert_eq_ ## bits; \
    cmp $0, %rax; \
    ASSERT(je); \
;

#define ASSERT_MEMCMP(label1, label2, const_size) \
    lea label1(%rip), %rdi; \
    lea label2(%rip), %rsi; \
    mov const_size, %rdx; \
    call assert_memcmp; \
    cmp $0, %rax; \
    ASSERT(je); \
;

/* Program entry point.
 *
 * Return with EXIT.
 *
 * Basically implements an x86_64 prologue:
 *
 * - save callee saved registers
 *   x86_64 explained at: https://stackoverflow.com/questions/18024672/what-registers-are-preserved-through-a-linux-x86-64-function-call/55207335#55207335
 * - save register arguments for later usage
 */
#define ENTRY \
.text; \
.global asm_main; \
asm_main: \
    push %rbp; \
    mov %rsp, %rbp; \
    push %r15; \
    push %r14; \
    push %r13; \
    push %r12; \
    push %rbx; \
    push %rdi; \
    sub $8, %rsp; \
asm_main_after_prologue: \
;

/* Meant to be called at the end of ENTRY.*
 *
 * Branching to "fail" makes tests fail with exit status 1.
 *
 * If EXIT is reached, the program ends successfully.
 */
#define EXIT \
    mov $0, %rax; \
    jmp pass; \
fail: \
    /* -0x30(%rbp) is argument 1 which we pushed at prologue */ \
    mov -0x30(%rbp), %rbx; \
    movl %eax, (%rbx); \
    mov $1, %rax; \
pass: \
    add $16, %rsp; \
    pop %rbx; \
    pop %r12; \
    pop %r13; \
    pop %r14; \
    pop %r15; \
    pop %rbp; \
    ret; \
;

#define FAIL \
    mov $__LINE__, %eax; \
    jmp fail; \
;

#endif
