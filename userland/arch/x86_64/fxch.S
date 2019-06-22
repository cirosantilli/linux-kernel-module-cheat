/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-x87-fpu-instructions */

#include <lkmc.h>

.data
    double_0_0: .double 0.0
    double_1_0: .double 1.0
LKMC_PROLOGUE
    fldz
    # ST0 = 0.0

    fld1
    # ST0 = 1.0
    # ST1 = 0.0

    # Swap ST0 and ST1.
    fxch %st(1)
    # ST0 = 0.0
    # ST1 = 1.0

    fldz
    # ST0 = 0.0
    # ST1 = 0.0
    # ST2 = 1.0

    fcomip %st(1)
    # ST0 = 0.0
    # ST1 = 1.0
    LKMC_ASSERT(je)

    # Swap ST0 and ST1.
    fxch %st(1)
    # ST0 = 1.0
    # ST1 = 0.0

    fld1
    # ST0 = 1.0
    # ST1 = 1.0
    # ST2 = 0.0

    fcomip %st(1)
    # ST0 = 1.0
    # ST1 = 0.0
    LKMC_ASSERT(je)
LKMC_EPILOGUE
