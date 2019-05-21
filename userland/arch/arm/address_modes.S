/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-addressing-modes */

#include "common.h"

LKMC_ENTRY

    /* Offset mode with immediate. Add 4 to the address register,
     * which ends up * reading myvar6 instead of myvar.
     */
    adr r4, myvar
    ldr r5, [r4, 4]
    LKMC_ASSERT_EQ(r5, 0x9ABCDEF0)
    /* r4 was not modified. */
    LKMC_ASSERT_EQ(r4, myvar)

    /* Pre-indexed mode: modify register, then use it. */
    adr r4, myvar
    ldr r5, [r4, 4]!
    LKMC_ASSERT_EQ(r5, 0x9ABCDEF0)
    /* r4 was modified. */
    LKMC_ASSERT_EQ(r4, myvar6)

    /* Post-indexed mode: use register, then modify it. */
    adr r4, myvar
    ldr r5, [r4], 4
    LKMC_ASSERT_EQ(r5, 0x12345678)
    /* r4 was modified. */
    LKMC_ASSERT_EQ(r4, myvar6)

    /* Offset in register. */
    adr r4, myvar
    mov r5, 4
    ldr r6, [r4, r5]
    LKMC_ASSERT_EQ(r6, 0x9ABCDEF0)

    /* Offset in shifted register:
     * r6 =
     * (r4 + (r5 << 1))
     * == *(myvar + (2 << 1))
     * == *(myvar + 4)
     */
    adr r4, myvar
    mov r5, 2
    ldr r6, [r4, r5, lsl 1]
    LKMC_ASSERT_EQ(r6, 0x9ABCDEF0)

LKMC_EXIT
myvar:
    .word 0x12345678
myvar6:
    .word 0x9ABCDEF0
