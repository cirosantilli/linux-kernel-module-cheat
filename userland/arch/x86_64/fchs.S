/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-x87-fpu-instructions */

#include <lkmc.h>

.data
    double_1:       .double  1.0
    double_minus_1: .double -1.0
LKMC_PROLOGUE
    /* -(1) == -1 */
    fldl double_1
    fchs
    fldl double_minus_1
    fcomip %st(1)
    LKMC_ASSERT(je)
    finit

    /* -(-1) == 1 */
    fldl double_minus_1
    fchs
    fldl double_1
    fcomip %st(1)
    LKMC_ASSERT(je)
    finit
LKMC_EPILOGUE
