/* https://github.com/cirosantilli/linux-kernel-module-cheat#assembly-simd
 *
 * Add a bunch of integers in one go.
 */

#include "common.h"

ENTRY
.data
    input0:    .long 0xF1F1F1F1, 0xF2F2F2F2, 0xF3F3F3F3, 0xF4F4F4F4
    input1:    .long 0x12121212, 0x13131313, 0x14141414, 0x15151515
    expect_4s: .long 0x04040403, 0x06060605, 0x08080807, 0x0A0A0A09
    expect_2d: .long 0x04040403, 0x06060606, 0x08080807, 0x0A0A0A0A
.bss
    output:    .skip 16
.text
#define TEST(size) \
    adr x0, input0; \
    ld1 {v0. ## size}, [x0]; \
    adr x1, input1; \
    ld1 {v1. ## size}, [x1]; \
    add v2. ## size, v0. ## size, v1. ## size; \
    adr x0, output; \
    st1 {v2. ## size}, [x0]; \
    ASSERT_MEMCMP(output, expect_ ## size, 0x10)

    /* 4x 32-bit */
    TEST(4s)
    /* 2x 64-bit */
    TEST(2d)
#undef TEST
EXIT
