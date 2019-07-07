/* https://cirosantilli.com/linux-kernel-module-cheat#x86-fma */

#include <lkmc.h>

LKMC_PROLOGUE
.data
    .align 16
    input0: .double 1.5,  2.5
    input1: .double 2.0,  4.0
    input2: .double 2.5,  3.5
    expect: .double 6.5, 16.5
.bss
    .align 16
    output: .skip 16
.text
    movaps input1, %xmm0
    movaps input0, %xmm1
    movaps input2, %xmm2
    /* xmm2 = xmm1 + (xmm0 * xmm2) */
    vfmadd132pd %xmm0, %xmm1, %xmm2
    movaps %xmm2, output
    LKMC_ASSERT_MEMCMP(output, expect, $0x10)
LKMC_EPILOGUE
