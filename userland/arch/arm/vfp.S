/* https://github.com/cirosantilli/arm-assembly-cheat#vfp
 * Adapted from: https://mindplusplus.wordpress.com/2013/06/27/arm-vfp-vector-programming-part-2-examples/ */

#include "common.h"

.data;
a1:
    .float 0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5
a2:
    .float 5.0, 5.5, 6.0, 6.5, 7.0, 7.5, 8.0, 8.5
sum:
    .skip 32
sum_expect:
    .float 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0

ENTRY
    /* Minimal single precision floating point example.
     * TODO: floating point representation constraints due to 4-byte instruction?
     */
    vmov s0, 1.5
    vmov s1, 2.5
    vadd.f32 s2, s0, s1
    vmov s3, 4.0
    /* Compare two floating point registers. Stores results in fpscr:
     * (floating point status and control register).
     */
    vcmp.f32 s2, s3
    /* Move the nzcv bits from fpscr to apsr */
    vmrs apsr_nzcv, fpscr
    /* This branch uses the Z bit of apsr, which was set accordingly. */
    ASSERT(beq)

    /* Now the same from memory with vldr and vstr. */
.data
my_float_0:
    .float 1.5
my_float_1:
    .float 2.5
my_float_sum_expect:
    .float 4.0
.bss
my_float_sum:
    .skip 4
.text
    ldr r0, =my_float_0
    vldr s0, [r0]
    ldr r0, =my_float_1
    vldr s1, [r0]
    vadd.f32 s2, s0, s1
    ldr r0, =my_float_sum
    vstr.f32 s2, [r0]
    ASSERT_MEMCMP(my_float_sum, my_float_sum_expect, 4)

#if 0
    /* We can't do pseudo vldr as for ldr, fails with:
     * Error: cannot represent CP_OFF_IMM relocation in this object file format
     * It works on ARMv8 however, so the relocation must have been added.
     */
    vldr s0, my_float_0
#endif

    /* Minimal double precision floating point example. */
    vmov.f64 d0, 1.5
    vmov.f64 d1, 2.5
    vadd.f64 d2, d0, d1
    vmov.f64 d3, 4.0
    vcmp.f64 d2, d3
    vmrs apsr_nzcv, fpscr
    ASSERT(beq)

    /* vmov can also move to general purpose registers.
     *
     * Just remember that we can't use float immediates with general purpose registers:
     * https://stackoverflow.com/questions/6514537/how-do-i-specify-immediate-floating-point-numbers-with-inline-assembly/52906126#52906126
     */
    mov r1, 2
    mov r0, 1
    vmov s0, r0
    vmov s1, s0
    vmov r1, s1
    ASSERT_EQ_REG(r0, r1)

    /* Now a more complex test function. */
    ldr r0, =sum
    ldr r1, =a1
    ldr r2, =a2
    mov r3, 8
    bl vec_sum
    /* The assert works easily because all floats used
     * have exact base-2 representation.
     */
    ASSERT_MEMCMP(sum, sum_expect, 0x20)
EXIT

/* void vec_sum(float *sum, float *a1, float *a2, int length) {
 *   int i;
 *   for (i=0; i &lt; length; i++)
 *     *(sum+i) = *(a1+i) + *(a2+i);
 * }
 */
vec_sum:
    /* Setup */
    push {r0, r1, r4, lr}
    push {r0, r1}
    mov r0, 1
    mov r1, 8
    bl reconfig
    pop {r0, r1}
    asr r3, 3

    /* Do the sum. */
1:
    fldmias r1!, {s8-s15}
    fldmias r2!, {s16-s23}
    vadd.f32 s24, s8, s16
    fstmias r0!, {s24-s31}
    subs r3, r3, 1
    bne 1b

    /* Teardown. */
    bl deconfig
    pop {r0, r1, r4, pc}

/* inputs:
 * r0: desired vector stride (1 or 2)
 * r1: desired vector length (min. 1, max. 8)
 * outputs: (none)
 * modified: r0, r1, FPSCR
 * notes:
 * r0 and r1 will be truncated before fitting into FPSCR
 */
reconfig:
    push {r0-r2}
    and r0, r0, 3
    eor r0, r0, 1
    sub r1, r1, 1
    and r1, r1, 7
    mov r0, r0, lsl 20
    orr r0, r0, r1, lsl 16
    vmrs r2, fpscr
    bic r2, 55*65536
    orr r2, r2, r0
    vmsr fpscr, r0
    pop {r0-r2}
    bx lr

deconfig:
    push {r0, r1, lr}
    mov r0, 1
    mov r1, 1
    bl reconfig
    pop {r0, r1, pc}
