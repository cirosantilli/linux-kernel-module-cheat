/* https://cirosantilli.com/linux-kernel-module-cheat#arm-data-processing-instructions
 *
 * Test. Same as ands, but don't store the result, just update flags.
 */

#include <lkmc.h>

LKMC_PROLOGUE

    /* 0x0F && 0xF0 == 0x00, so beq. */
    mov r0, 0x0F
    tst r0, 0xF0
    LKMC_ASSERT(beq)

    /* bne */
    mov r0, 0xFF
    tst r0, 0x0F
    LKMC_ASSERT(bne)
    /* r0 was not modified. */
    LKMC_ASSERT_EQ(r0, =0xFF)

LKMC_EPILOGUE
