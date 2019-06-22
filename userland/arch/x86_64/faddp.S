/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-x87-fpu-instructions */

#include <lkmc.h>

.data
    double_1_5: .double 1.5
    double_2_5: .double 2.5
    double_4_0: .double 4.0
LKMC_PROLOGUE
    fldl double_1_5
    /* FPU stack after operation:
     * ST0 == 1.5 */

    fldl double_2_5
    /* FPU stack after operation:
     * ST0 == 2.5
     * ST1 == 1.5 */

    /* ST0 = ST0 + ST1
     * Pop ST0. */
    faddp %st, %st(1)
    /* FPU stack after operation:
     * ST0 == 4.0 */

    fldl double_4_0
    /* FPU stack after operation:
     * ST0 == 4.0
     * ST1 == 4.0 */

    /* Compare ST0 == ST1
     * Pop ST0. */
    fcomip %st(1)
    /* FPU stack after operation:
     * ST0 == 4.0 */
    LKMC_ASSERT(je)
LKMC_EPILOGUE
