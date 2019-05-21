/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-vadd-instruction */

#include "common.h"

.bss
    output:      .skip 16
LKMC_ENTRY
    /* Integer. */
.data
    input0_u:    .long 0xF1F1F1F1, 0xF2F2F2F2, 0xF3F3F3F3, 0xF4F4F4F4
    input1_u:    .long 0x12121212, 0x13131313, 0x14141414, 0x15151515
    expect_u_32: .long 0x04040403, 0x06060605, 0x08080807, 0x0A0A0A09
    expect_u_64: .long 0x04040403, 0x06060606, 0x08080807, 0x0A0A0A0A
.text
#define TEST(size) \
    ldr r0, =input0_u; \
    vld1. ## size {q0}, [r0]; \
    ldr r0, =input1_u; \
    vld1. ## size {q1}, [r0]; \
    vadd.u ## size q2, q0, q1; \
    ldr r0, =output; \
    vst1.u ## size {q2}, [r0]; \
    LKMC_ASSERT_MEMCMP(output, expect_u_ ## size, 0x10)

    /* vadd.u32
     *
     * Add 4x 32-bit unsigned integers in one go.
     *
     * q means quad (128-bits)
     *
     * u32 means that we treat memory as uint32_t types.
     *
     * 4 is deduced: in 128 bits you can fit 4x u32.
     */
    TEST(32)
    /* 2x 64-bit */
    TEST(64)
#undef TEST

    /* Floating point. */
.data
    input0_f_32: .float 1.5, 2.5,  3.5,  4.5
    input1_f_32: .float 5.5, 6.5,  7.5,  8.5
    expect_f_32: .float 7.0, 9.0, 11.0, 13.0
    input0_f_64: .double 1.5, 2.5
    input1_f_64: .double 5.5, 6.5
    expect_f_64: .double 7.0, 9.0
.text
#define TEST(size) \
    ldr r0, =input0_f_ ## size; \
    vld1. ## size {q0}, [r0]; \
    ldr r0, =input1_f_ ## size; \
    vld1. ## size {q1}, [r0]; \
    vadd.f ## size q2, q0, q1; \
    ldr r0, =output; \
    vst1. ## size {q2}, [r0]; \
    LKMC_ASSERT_MEMCMP(output, expect_f_ ## size, 0x10)

    /* 4x 32-bit. */
    TEST(32)
#if 0
    /* vadd.f64: 2x 64-bit float add: appears not possible.
     * https://stackoverflow.com/questions/36052564/does-arm-support-simd-operations-for-64-bit-floating-point-numbers
     *
     * Fails with:
     * bad type in Neon instruction -- `vadd.f64 q2,q0,q1' */
     */
    TEST(64)
#endif
#undef TEST
LKMC_EXIT
