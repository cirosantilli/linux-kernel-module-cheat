#ifndef LKMC_ARM_H
#define LKMC_ARM_H

#if defined(__ASSEMBLER__)
.syntax unified
#endif

#define LKMC_ASSERT_EQ(reg, const) \
    mov r0, reg; \
    ldr r1, const; \
    ldr r2, =__LINE__; \
    bl lkmc_assert_eq_32; \
;

#define LKMC_ASSERT_EQ_REG(reg1, reg2) \
    str reg2, [sp, -4]!; \
    mov r0, reg1; \
    ldr r1, [sp], 4; \
    ldr r2, =__LINE__; \
    bl lkmc_assert_eq_32; \
;

#define LKMC_ASSERT_FAIL \
    ldr r0, =__LINE__; \
    bl lkmc_assert_fail; \
;

#define LKMC_ASSERT_MEMCMP(label1, label2, size) \
    ldr r0, =label1; \
    ldr r1, =label2; \
    ldr r2, size; \
    ldr r3, =__LINE__; \
    bl lkmc_assert_memcmp; \
;

/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-calling-convention */
#define LKMC_EPILOGUE \
    add sp, 16; \
    ldmia sp!, {r4-r12, lr}; \
    mov r0, 0; \
    bx lr; \
;

/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-calling-convention */
#define LKMC_PROLOGUE \
.text; \
.global main; \
main: \
    stmdb sp!, {r0-r12, lr}; \
main_after_prologue: \
;

#endif
