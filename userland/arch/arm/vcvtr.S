/* https://github.com/cirosantilli/arm-assembly-cheat#vcvtrr */

#include "common.h"

ENTRY
.data
    vcvtr_0:                    .float 1.25, 2.5, 3.75, 4.0
    vcvtr_expect_zero:          .word  1,    2,   3,    4
    vcvtr_expect_plus_infinity: .word  2,    3,   4,    4
.bss
    vcvtr_result_zero:          .skip 0x10
    vcvtr_result_plus_infinity: .skip 0x10
.text
    ldr r0, =vcvtr_0
    vld1.32 {q0}, [r0]

    /* zero */
    vmrs r0, fpscr
    orr r0, r0, (3 << 22)
    vmsr fpscr, r0
    vcvtr.u32.f32 q1, q0
    ldr r0, =vcvtr_result_zero
    vst1.32 {q1}, [r0]
    ASSERT_MEMCMP(
        vcvtr_result_zero,
        vcvtr_expect_zero,
        0x10
    )

#if 0
    /* TODO why is this not working? Rounds to zero still. */
    /* plus infinity */
    vmrs r0, fpscr
    mov r1, 1
    bfi r0, r1, 22, 2
    vmsr fpscr, r0
    vcvtr.u32.f32 q1, q0
    ldr r0, =vcvtr_result_plus_infinity
    vst1.32 {q1}, [r0]
    ASSERT_MEMCMP(
        vcvtr_result_plus_infinity,
        vcvtr_expect_plus_infinity,
        0x10
    )
#endif
EXIT
