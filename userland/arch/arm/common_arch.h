#ifndef COMMON_ARCH_H
#define COMMON_ARCH_H

.syntax unified

/* Assert that a register equals a constant.
 * * reg: the register to check. Can be r0-r10, but not r11. r11 is overwritten.
 * * const: the constant to compare to. Only works for literals or labels, not for registers.
 *          For register / register comparision, use ASSERT_EQ_REG.
 */
#define ASSERT_EQ(reg, const) \
    ldr r11, =const; \
	cmp reg, r11; \
	ASSERT(beq); \
;

/* Assert that two arrays are the same. */
#define ASSERT_MEMCMP(s1, s2, n) \
	MEMCMP(s1, s2, n); \
	ASSERT_EQ(r0, 0); \
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
