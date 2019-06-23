/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-sse-packed-arithmetic-instructions */

#include <lkmc.h>

LKMC_PROLOGUE
.data
    /* Unlike MOVAPS, we don't need to align memory here. */
    input: .float 1.5, 2.5, 3.5, 4.5
.bss
    output: .skip 16
.text
    movups input, %xmm0
    movups %xmm0, %xmm1
    movups %xmm1, output
    LKMC_ASSERT_MEMCMP(input, output, $16)
LKMC_EPILOGUE
