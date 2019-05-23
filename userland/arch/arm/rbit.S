/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-data-processing-instructions
 *
 * Reverse bit order.
 */

#include <lkmc.h>

LKMC_PROLOGUE
    ldr r0,      =0b00000001001000110100010101100101
    rbit r1, r0
    LKMC_ASSERT_EQ(r1, =0b10100110101000101100010010000000)
LKMC_EPILOGUE
