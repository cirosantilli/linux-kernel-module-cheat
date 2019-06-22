/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-x87-fpu-instructions */

#include <lkmc.h>

.data
    double_1_41: .double 1.41
    double_1_42: .double 1.42
    double_2_0: .double 2.0
    double_4_0: .double 4.0
LKMC_PROLOGUE
    /* sqrt(4) == 4 */
    fldl double_4_0
    fsqrt
    fldl double_2_0
    fcomip %st(1)
    LKMC_ASSERT(je)

    /* 1.41 < sqrt(2) < 1.42 */
    fsqrt
    fldl double_1_41
    fcomip %st(1)
    LKMC_ASSERT(jbe)
    fldl double_1_42
    fcomip %st(1)
    LKMC_ASSERT(jae)
LKMC_EPILOGUE
