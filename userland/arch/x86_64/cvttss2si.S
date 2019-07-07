/* https://cirosantilli.com/linux-kernel-module-cheat#x86-sse-packed-arithmetic-instructions */

#include <lkmc.h>

LKMC_PROLOGUE
.data
    .align 16
    input_2_5: .float 2.5
    input_minus_2_5: .float -2.5
.text
    /* Positive input. */
    movss input_2_5, %xmm0
    cvttss2si %xmm0, %eax
    LKMC_ASSERT_EQ_32(%eax, $2)

    /* Negative input. */
    movss input_minus_2_5, %xmm0
    cvttss2si %xmm0, %eax
    LKMC_ASSERT_EQ_32(%eax, $-2)
LKMC_EPILOGUE
