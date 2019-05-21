/* https://github.com/cirosantilli/linux-kernel-module-cheat#vfp
 * Adapted from: https://mindplusplus.wordpress.com/2013/06/27/arm-vfp-vector-programming-part-2-examples/ */

#include "common.h"

LKMC_ENTRY
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
    LKMC_ASSERT(beq)

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
    LKMC_ASSERT_MEMCMP(my_float_sum, my_float_sum_expect, 4)

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
    LKMC_ASSERT(beq)

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
    LKMC_ASSERT_EQ_REG(r0, r1)
LKMC_EXIT
