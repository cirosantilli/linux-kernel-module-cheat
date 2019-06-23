/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-sse-packed-arithmetic-instructions
 *
 * Add a few floating point numbers in one go (P == packaged).
 */

#include <lkmc.h>

LKMC_PROLOGUE
.data
    .align 16
    addps_input0: .float 1.5, 2.5,  3.5,  4.5
    addps_input1: .float 5.5, 6.5,  7.5,  8.5
    addps_expect: .float 7.0, 9.0, 11.0, 13.0
    addpd_input0: .double 1.5, 2.5
    addpd_input1: .double 5.5, 6.5
    addpd_expect: .double 7.0, 9.0
.bss
    .align 16
    output:       .skip 16
.text
    /* 4x 32-bit */
    movaps addps_input0, %xmm0
    movaps addps_input1, %xmm1
    addps %xmm1, %xmm0
    movaps %xmm0, output
    LKMC_ASSERT_MEMCMP(output, addps_expect, $0x10)

    /* 2x 64-bit */
    movaps addpd_input0, %xmm0
    movaps addpd_input1, %xmm1
    addpd %xmm1, %xmm0
    movaps %xmm0, output
    LKMC_ASSERT_MEMCMP(output, addpd_expect, $0x10)
LKMC_EPILOGUE
