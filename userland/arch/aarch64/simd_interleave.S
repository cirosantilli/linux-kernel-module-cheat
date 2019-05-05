/* https://github.com/cirosantilli/arm-assembly-cheat#simd-interleaving */

#include "common.h"

ENTRY
.data
    u32_interleave: .word \
        0x11111111, 0x55555555, \
        0x22222222, 0x66666666, \
        0x33333333, 0x77777777, \
        0x44444444, 0x88888888
    u32_interleave_sum_expect: .word \
        0x66666666, \
        0x88888888, \
        0xAAAAAAAA, \
        0xCCCCCCCC
.bss
    u32_interleave_sum: .skip 16
.text
    adr x0, u32_interleave
    ld2 {v0.4s, v1.4s}, [x0]
    add v2.4s, v0.4s, v1.4s
    adr x0, u32_interleave_sum
    st1 {v2.4s}, [x0]
    ASSERT_MEMCMP(u32_interleave_sum, u32_interleave_sum_expect, 0x10)
EXIT
