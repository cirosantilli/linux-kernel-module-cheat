/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-x87-fpu-instructions */

#include <lkmc.h>

.data
    double_1_0: .double 1.0
LKMC_PROLOGUE
    fld1
    fldl double_1_0
    fcomip %st(1)
    LKMC_ASSERT(je)
LKMC_EPILOGUE
