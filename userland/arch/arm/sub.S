/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-data-processing-instructions
 *
 * Subtraction.
 */

#include <lkmc.h>

LKMC_PROLOGUE
    /* 3 - 2 == 1 , register version.*/
    mov r0, 3
    mov r1, 2
    sub r0, r0, r1
    LKMC_ASSERT_EQ(r0, =1)
LKMC_EPILOGUE
