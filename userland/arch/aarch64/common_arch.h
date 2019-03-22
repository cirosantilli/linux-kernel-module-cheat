#ifndef COMMON_ARCH_H
#define COMMON_ARCH_H

#define ASSERT_EQ(reg, const) \
    ldr x11, =const; \
	cmp reg, x11; \
	ASSERT(beq); \
;

#define ASSERT_MEMCMP(s1, s2, n) \
	MEMCMP(s1, s2, n); \
	ASSERT_EQ(x0, 0); \
;

#define ENTRY \
.text; \
.global asm_main; \
asm_main: \
    sub  sp, sp, 0xA0; \
    stp  x29, x30, [sp]; \
    stp  x27, x28, [sp, 0x10]; \
    stp  x25, x26, [sp, 0x20]; \
    stp  x23, x24, [sp, 0x30]; \
    stp  x21, x22, [sp, 0x40]; \
    stp  x19, x20, [sp, 0x50]; \
    stp  x6, x7, [sp, 0x60]; \
    stp  x4, x5, [sp, 0x70]; \
    stp  x2, x3, [sp, 0x80]; \
    stp  x0, x1, [sp, 0x90]; \
asm_main_after_prologue: \
;

#define EXIT \
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

#define FAIL \
    ldr w0, =__LINE__; \
    b fail; \
;

#define MEMCMP(s1, s2, n) \
    adr x0, s1; \
    adr x1, s2; \
    ldr x2, =n; \
    bl memcmp; \
;

#endif
