/* https://github.com/cirosantilli/linux-kernel-module-cheat#simd-assembly
 *
 * Add a bunch of floating point numbers in one go.
 */

#include "common.h"

ENTRY
.data
    input0_4s: .float 1.5, 2.5,  3.5,  4.5
    input1_4s: .float 5.5, 6.5,  7.5,  8.5
    expect_4s: .float 7.0, 9.0, 11.0, 13.0
    input0_2d: .double 1.5, 2.5
    input1_2d: .double 5.5, 6.5
    expect_2d: .double 7.0, 9.0
.bss
    output:    .skip 16
.text
#define TEST(size) \
    adr x0, input0_ ## size; \
    ld1 {v0. ## size}, [x0]; \
    adr x1, input1_ ## size; \
    ld1 {v1. ## size}, [x1]; \
    fadd v2. ## size, v0. ## size, v1. ## size; \
    adr x0, output; \
    st1 {v2. ## size}, [x0]; \
    ASSERT_MEMCMP(output, expect_ ## size, 0x10)

    /* 4x 32-bit */
    TEST(4s)
    /* 2x 64-bit */
    TEST(2d)
#undef TEST
EXIT
