/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-sse-data-transfer-instructions */

#include <lkmc.h>

.data
    input:  .float 1.5
.bss
    output: .skip 4
LKMC_PROLOGUE
    movss input, %xmm0
    movss %xmm0, %xmm1
    movss %xmm1, output
    LKMC_ASSERT_MEMCMP(input, output, $4)
LKMC_EPILOGUE
