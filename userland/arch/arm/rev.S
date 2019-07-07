/* https://cirosantilli.com/linux-kernel-module-cheat#arm-data-processing-instructions
 *
 * Reverse byte order.
 */

#include <lkmc.h>

LKMC_PROLOGUE
    /* All bytes in register. */
    ldr r0, =0x11223344
    rev r1, r0
    LKMC_ASSERT_EQ(r1, =0x44332211)

    /* Groups of 16-bits. */
    ldr r0, =0x11223344
    rev16 r1, r0
    LKMC_ASSERT_EQ(r1, =0x22114433)
LKMC_EPILOGUE
