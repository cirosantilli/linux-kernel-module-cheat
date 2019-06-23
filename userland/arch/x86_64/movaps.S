/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-sse-packed-arithmetic-instructions */

#include <lkmc.h>

LKMC_PROLOGUE
.data
    /* Ensure that the memory is 16-byte aligned. */
    .align 16
    input: .float 1.5, 2.5, 3.5, 4.5
.bss
    .align 16
    output: .skip 16
.text
    movaps input, %xmm0
    movaps %xmm0, %xmm1
    movaps %xmm1, output
    LKMC_ASSERT_MEMCMP(input, output, $16)
LKMC_EPILOGUE
