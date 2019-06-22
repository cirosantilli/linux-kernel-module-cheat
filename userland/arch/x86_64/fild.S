/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-x87-fpu-instructions */

#include <lkmc.h>

.data
    double_10_0: .double 10.0
.bss
    double_10_0_2: .skip 8
LKMC_PROLOGUE
    movl $10, double_10_0_2
    fildl double_10_0_2
    fldl double_10_0
    fcomip %st(1)
    LKMC_ASSERT(je)
    finit
LKMC_EPILOGUE
