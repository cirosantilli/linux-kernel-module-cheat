/* https://github.com/cirosantilli/arm-assembly-cheat/blob/c19e187e98e99f2f4a042783ca238aa4eb2292ab/v8/floating_point.S */

.global main
main:
    /* 1.5 + 2.5 == 4.0 */
    fmov d0, #1.5
    fmov d1, #2.5
    fadd d2, d0, d1
    fmov d3, #4.0
    fcmp d2, d3
    beq 1f
    bl assert_fail
1:

    /* Now in 32-bit. */
    fmov s0, #1.5
    fmov s1, #2.5
    fadd s2, s0, s1
    fmov s3, #4.0
    fcmp s2, s3
    beq 1f
    bl assert_fail
1:

    ret
