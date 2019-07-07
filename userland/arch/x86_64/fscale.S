/* https://cirosantilli.com/linux-kernel-module-cheat#x86-x87-fpu-instructions */

#include <lkmc.h>

.data
    double_1_0: .double 1.0
    double_2_5: .double 2.5
    double_4_0: .double 4.0
LKMC_PROLOGUE
    fldl double_4_0
    /* ST0 = 4.0 */

    fldl double_2_5
    /* ST0 = 2.5
     * ST1 = 4.0 */

    fldl double_1_0
    /* ST0 = 1.0
     * ST1 = 2.5
     * ST2 = 4.0 */

    /* ST0 = 1 * 2 ^ (RoundTowardZero(2.5))
     *     = 1 * 2 ^ 2
     *     = 4 */
    fscale
    /* ST0 = 4.0
     * ST1 = 2.5
     * ST2 = 4.0 */

    fcomip %st(2)
    /* ST0 = 4.0
     * ST1 = 2.5 */
    LKMC_ASSERT(je)
LKMC_EPILOGUE
