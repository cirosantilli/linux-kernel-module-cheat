/* https://github.com/cirosantilli/linux-kernel-module-cheat#userland-assembly-c-standard-library */

#ifndef COMMON_ARCH_H
#define COMMON_ARCH_H

.syntax unified

/* Assert that a register equals a constant.
 * * reg: the register to check
 * * const: the constant to compare to. Only works for literals or labels, not for registers.
 *          For register / register comparison, use LKMC_ASSERT_EQ_REG.
 */
#define LKMC_ASSERT_EQ(reg, const) \
    mov r0, reg; \
    ldr r1, =const; \
    LKMC_ASSERT_EQ_DO; \
;

#define LKMC_ASSERT_EQ_DO \
    bl lkmc_assert_eq_32; \
    cmp r0, 0; \
    LKMC_ASSERT(beq); \
;

#define LKMC_ASSERT_EQ_REG(reg1, reg2) \
    str reg2, [sp, -4]!; \
    mov r0, reg1; \
    ldr r1, [sp], 4; \
    LKMC_ASSERT_EQ_DO; \
;

/* Assert that two arrays are the same. */
#define LKMC_ASSERT_MEMCMP(label1, label2, const_size) \
    ldr r0, =label1; \
    ldr r1, =label2; \
    ldr r2, =const_size; \
    bl lkmc_assert_memcmp; \
    cmp r0, 0; \
    LKMC_ASSERT(beq); \
;

/* Store all callee saved registers, and LR in case we make further BL calls.
 *
 * Also save the input arguments r0-r3 on the stack, so we can access them later on,
 * despite those registers being overwritten.
 */
#define LKMC_ENTRY \
.text; \
.global asm_main; \
asm_main: \
    stmdb sp!, {r0-r12, lr}; \
asm_main_after_prologue: \
;

/* Meant to be called at the end of LKMC_ENTRY.*
 *
 * Branching to "fail" makes tests fail with exit status 1.
 *
 * If LKMC_EXIT is reached, the program ends successfully.
 *
 * Restore LR and bx jump to it to return from asm_main.
 */
#define LKMC_EXIT \
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
#define LKMC_FAIL \
    ldr r0, =__LINE__; \
    b fail; \
;

#endif
