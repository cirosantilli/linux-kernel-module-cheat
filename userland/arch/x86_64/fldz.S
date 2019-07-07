/* https://cirosantilli.com/linux-kernel-module-cheat#x86-x87-fpu-instructions */

#include <lkmc.h>

.data
    double_0_0: .double 0.0
LKMC_PROLOGUE
    fldz
    fldl double_0_0
    fcomip %st(1)
    LKMC_ASSERT(je)
LKMC_EPILOGUE
