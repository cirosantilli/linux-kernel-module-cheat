/* https://github.com/cirosantilli/arm-assembly-cheat#movk */

#include "common.h"

ENTRY
    movk x0, 0x4444, lsl 0
    movk x0, 0x3333, lsl 16
    movk x0, 0x2222, lsl 32
    movk x0, 0x1111, lsl 48
    ASSERT_EQ(x0, 0x1111222233334444)

    /* Set a label (addresses are 48-bit) with immediates:
     *
     * * https://stackoverflow.com/questions/38570495/aarch64-relocation-prefixes
     * * https://sourceware.org/binutils/docs-2.26/as/AArch64_002dRelocations.html
     *
     * This could be used if the label is too far away for
     * adr relative addressing.
     */
    movz x0, :abs_g2:label     /* bits 32-47, overflow check */
    movk x0, :abs_g1_nc:label  /* bits 16-31, no overflow check */
    movk x0, :abs_g0_nc:label  /* bits  0-15, no overflow check */
    adr x1, label
label:
    ASSERT_EQ_REG(x0, x1)
EXIT
