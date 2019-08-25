/* https://cirosantilli.com/linux-kernel-module-cheat#arm-sve */

#include <lkmc.h>

.data
    .align 16
    x:        .double 1.5,  2.5,  3.5,  4.5
    y:        .double 5.0,  6.0,  7.0,  8.0
    y_expect: .double 8.0, 11.0, 14.0, 17.0
    a:        .double 2.0
    n:        .word   4

LKMC_PROLOGUE
    adr x0, x
    adr x1, y
    adr x2, a
    adr x3, n
    bl daxpy
    LKMC_ASSERT_MEMCMP(y, y_expect, =0x20)
LKMC_EPILOGUE

/* Multiply by a scalar and add.
 *
 * Operation:
 *
 * ....
 * Y += a * X + Y
 * ....
 *
 * C signature:
 *
 * ....
 * void daxpy(double *x, double *y, double a, int n)
 * ....
 *
 * The name "daxpy" comes from LAPACK:
 * http://www.netlib.org/lapack/explore-html/de/da4/group__double__blas__level1_ga8f99d6a644d3396aa32db472e0cfc91c.html
 *
 * Adapted from: https://alastairreid.github.io/papers/sve-ieee-micro-2017.pdf
 */
daxpy:
    ldrsw x3, [x3]
    mov x4, 0
    whilelt p0.d, x4, x3
    ld1rd z0.d, p0/z, [x2]
.Lloop:
    ld1d z1.d, p0/z, [x0, x4, lsl 3]
    ld1d z2.d, p0/z, [x1, x4, lsl 3]
    fmla z2.d, p0/m, z1.d, z0.d
    st1d z2.d, p0, [x1, x4, lsl 3]
    incd x4
    whilelt p0.d, x4, x3
    b.first .Lloop
    ret
