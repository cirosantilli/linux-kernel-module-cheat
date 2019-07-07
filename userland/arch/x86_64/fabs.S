/* https://cirosantilli.com/linux-kernel-module-cheat#x86-x87-fpu-instructions */

#include <lkmc.h>

.data
    double_1_0:       .double  1.0
    double_minus_1_0: .double -1.0
LKMC_PROLOGUE
    /* |-1| == 1 */
    fldl double_minus_1_0
    fabs
    fldl double_1_0
    fcomip %st(1)
    LKMC_ASSERT(je)
    finit

    /* |1| == 1 */
    fldl double_1_0
    fabs
    fldl double_1_0
    fcomip %st(1)
    LKMC_ASSERT(je)
    finit
LKMC_EPILOGUE
