/* https://github.com/cirosantilli/arm-assembly-cheat#advanced-simd-instructions */

#include "common.h"

ENTRY
    /* 4x 32-bit integer add.
     *
     * s stands for single == 32 bits.
     *
     * 1 in ld1 means to load just one register, see:
     * https://github.com/cirosantilli/arm-assembly-cheat#simd-interleaving
     */
.data
    u32_0:          .word 0xF111F111, 0xF222F222, 0xF333F333, 0xF444F444
    u32_1:          .word 0x15551555, 0x16661666, 0x17771777, 0x18881888
    u32_sum_expect: .word 0x06670666, 0x08890888, 0x0AAB0AAA, 0x0CCD0CCC
.bss
    u32_sum:        .skip 16
.text
    adr x0, u32_0
    ld1 {v0.4s}, [x0]
    adr x1, u32_1
    ld1 {v1.4s}, [x1]
    add v2.4s, v0.4s, v1.4s
    adr x0, u32_sum
    st1 {v2.4s}, [x0]
    ASSERT_MEMCMP(u32_sum, u32_sum_expect, 0x10)

    /* 2x 64-bit integer add.
     *
     * d stands for double == 64 bits.
     */
.data
    u64_0:          .quad 0xF1111111F1111111, 0xF2222222F2222222
    u64_1:          .quad 0x1555555515555555, 0x1666666616666666
    u64_sum_expect: .quad 0x0666666706666666, 0x0888888908888888
.bss
    u64_sum: .skip 16
.text
    adr x0, u64_0
    ld1 {v0.2d}, [x0]
    adr x1, u64_1
    ld1 {v1.2d}, [x1]
    add v2.2d, v0.2d, v1.2d
    adr x0, u64_sum
    st1 {v2.2d}, [x0]
    ASSERT_MEMCMP(u64_sum, u64_sum_expect, 0x10)

    /* 4x 32-bit float add.
     *
     * The only difference between the integer point version
     * is that we use fadd instead of add.
     */
.data
    f32_0:          .float 1.5, 2.5,  3.5,  4.5
    f32_1:          .float 5.5, 6.5,  7.5,  8.5
    f32_sum_expect: .float 7.0, 9.0, 11.0, 13.0
.bss
    f32_sum: .skip 16
.text
    adr x0, f32_0
    ld1 {v0.4s}, [x0]
    adr x1, f32_1
    ld1 {v1.4s}, [x1]
    fadd v2.4s, v0.4s, v1.4s
    adr x0, f32_sum
    st1 {v2.4s}, [x0]
    ASSERT_MEMCMP(f32_sum, f32_sum_expect, 0x10)

    /* 2x 64-bit float add. */
.data
    f64_0:          .double 1.5, 2.5
    f64_1:          .double 5.5, 6.5
    f64_sum_expect: .double 7.0, 9.0
.bss
    f64_sum: .skip 16
.text
    adr x0, f64_0
    ld1 {v0.2d}, [x0]
    adr x1, f64_1
    ld1 {v1.2d}, [x1]
    fadd v2.2d, v0.2d, v1.2d
    adr x0, f64_sum
    st1 {v2.2d}, [x0]
    ASSERT_MEMCMP(f64_sum, f64_sum_expect, 0x10)
EXIT
