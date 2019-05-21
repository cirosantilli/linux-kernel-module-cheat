/* https://github.com/cirosantilli/linux-kernel-module-cheat#userland-assembly-c-standard-library */

#ifndef COMMON_ARCH_H
#define COMMON_ARCH_H

#define LKMC_ASSERT_EQ(reg, const) \
    mov x0, reg; \
    ldr x1, =const; \
    LKMC_ASSERT_EQ_DO(64); \
;

#define LKMC_ASSERT_EQ_DO(bits) \
    bl lkmc_assert_eq_ ## bits; \
    cmp x0, 0; \
    LKMC_ASSERT(beq); \
;

#define LKMC_ASSERT_EQ_REG(reg1, reg2) \
    str reg2, [sp, -16]!; \
    mov x0, reg1; \
    ldr x1, [sp], 16; \
    LKMC_ASSERT_EQ_DO(64); \
;

#define LKMC_ASSERT_EQ_REG_32(reg1, reg2) \
    str reg2, [sp, -4]!; \
    mov w0, reg1; \
    ldr w1, [sp], 4; \
    LKMC_ASSERT_EQ_DO(32); \
;

#define LKMC_ASSERT_MEMCMP(label1, label2, const_size) \
    adr x0, label1; \
    adr x1, label2; \
    ldr x2, =const_size; \
    bl lkmc_assert_memcmp; \
    cmp x0, 0; \
    LKMC_ASSERT(beq); \
;

#define LKMC_ENTRY \
.text; \
.global asm_main; \
asm_main: \
    sub sp, sp, 0xA0; \
    stp x29, x30, [sp]; \
    stp x27, x28, [sp, 0x10]; \
    stp x25, x26, [sp, 0x20]; \
    stp x23, x24, [sp, 0x30]; \
    stp x21, x22, [sp, 0x40]; \
    stp x19, x20, [sp, 0x50]; \
    stp  x6,  x7, [sp, 0x60]; \
    stp  x4,  x5, [sp, 0x70]; \
    stp  x2,  x3, [sp, 0x80]; \
    stp  x0,  x1, [sp, 0x90]; \
asm_main_after_prologue: \
;

#define LKMC_EXIT \
    mov w0, 0; \
    mov w1, 0; \
    b pass; \
fail: \
    ldr x1, [sp, 0x90]; \
    str w0, [x1]; \
    mov w0, 1; \
pass: \
    ldp x19, x20, [sp, 0x50]; \
    ldp x21, x22, [sp, 0x40]; \
    ldp x23, x24, [sp, 0x30]; \
    ldp x25, x26, [sp, 0x20]; \
    ldp x27, x28, [sp, 0x10]; \
    ldp x29, x30, [sp]; \
    add sp, sp, 0xA0; \
    ret; \
;

#define LKMC_FAIL \
    ldr w0, =__LINE__; \
    b fail; \
;

#endif
