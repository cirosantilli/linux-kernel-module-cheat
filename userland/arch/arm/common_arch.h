/* https://github.com/cirosantilli/linux-kernel-module-cheat#userland-assembly-c-standard-library */

#ifndef COMMON_ARCH_H
#define COMMON_ARCH_H

.syntax unified

/* Assert that a register equals a constant.
 * * reg: the register to check
 * * const: the constant to compare to. Only works for literals or labels, not for registers.
 *          For register / register comparison, use ASSERT_EQ_REG.
 */
#define ASSERT_EQ(reg, const) \
    mov r0, reg; \
    ldr r1, =const; \
    ASSERT_EQ_DO; \
;

#define ASSERT_EQ_DO \
    bl assert_eq_32; \
    cmp r0, 0; \
    ASSERT(beq); \
;

#define ASSERT_EQ_REG(reg1, reg2) \
    str reg2, [sp, -4]!; \
    mov r0, reg1; \
    ldr r1, [sp], 4; \
    ASSERT_EQ_DO; \
;

/* Assert that two arrays are the same. */
#define ASSERT_MEMCMP(label1, label2, const_size) \
    ldr r0, =label1; \
    ldr r1, =label2; \
    ldr r2, =const_size; \
    bl assert_memcmp; \
    cmp r0, 0; \
    ASSERT(beq); \
;

/* Store all callee saved registers, and LR in case we make further BL calls.
 *
 * Also save the input arguments r0-r3 on the stack, so we can access them later on,
 * despite those registers being overwritten.
 */
#define ENTRY \
.text; \
.global asm_main; \
asm_main: \
    stmdb sp!, {r0-r12, lr}; \
asm_main_after_prologue: \
;

/* Meant to be called at the end of ENTRY.*
 *
 * Branching to "fail" makes tests fail with exit status 1.
 *
 * If EXIT is reached, the program ends successfully.
 *
 * Restore LR and bx jump to it to return from asm_main.
 */
#define EXIT \
    mov r0, 0; \
    mov r1, 0; \
    b pass; \
fail: \
    ldr r1, [sp]; \
    str r0, [r1]; \
    mov r0, 1; \
pass: \
    add sp, 16; \
    ldmia sp!, {r4-r12, lr}; \
    bx lr; \
;

/* Always fail. */
#define FAIL \
    ldr r0, =__LINE__; \
    b fail; \
;

#define MEMCMP(s1, s2, n) \
    ldr r0, =s1; \
    ldr r1, =s2; \
    ldr r2, =n; \
    bl memcmp; \
;

#endif
