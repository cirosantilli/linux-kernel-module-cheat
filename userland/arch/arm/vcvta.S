/* https://github.com/cirosantilli/arm-assembly-cheat#vcvta */

#include "common.h"

ENTRY
    /* SIMD positive. */
.data
    vcvta_positive_0:      .float 1.25, 2.5, 3.75, 4.0
    vcvta_positive_expect: .word  1,    3,   4,    4
.bss
    vcvta_positive_result: .skip 0x10
.text
    ldr r0, =vcvta_positive_0
    vld1.32 {q0}, [r0]
    vcvta.u32.f32 q1, q0
    ldr r0, =vcvta_positive_result
    vst1.32 {q1}, [r0]
    ASSERT_MEMCMP(
        vcvta_positive_result,
        vcvta_positive_expect,
        0x10
    )

    /* SIMD negative. */
.data
    vcvta_negative_0:      .float -1.25, -2.5, -3.75, -4.0
    vcvta_negative_expect: .word  -1,    -3,   -4,    -4
.bss
    vcvta_negative_result: .skip 0x10
.text
    ldr r0, =vcvta_negative_0
    vld1.32 {q0}, [r0]
    vcvta.s32.f32 q1, q0
    ldr r0, =vcvta_negative_result
    vst1.32 {q1}, [r0]
    ASSERT_MEMCMP(
        vcvta_negative_result,
        vcvta_negative_expect,
        0x10
    )
EXIT
