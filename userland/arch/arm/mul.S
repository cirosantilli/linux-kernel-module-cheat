/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-data-processing-instructions
 *
 * Multiplication.
 */

#include <lkmc.h>

LKMC_PROLOGUE
    /* 2 * 3 = 6 */
    mov r0, 0
    mov r1, 2
    mov r2, 3
    mul r1, r2
    LKMC_ASSERT_EQ(r1, =6)
LKMC_EPILOGUE
