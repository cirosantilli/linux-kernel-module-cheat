/* https://cirosantilli.com/linux-kernel-module-cheat#x86-x87-fpu-instructions */

#include <lkmc.h>

.data
    double_1_5: .double 1.5
.bss
    double_1_5_2: .skip 8
LKMC_PROLOGUE
#if 0
    /* Error: junk `.5' after expression */
    movq $1.5, double_1_5_2
    fldl double_1_5
    fldl double_1_5_2
    fcomi %st(1)
    LKMC_ASSERT(je)
#endif
LKMC_EPILOGUE
