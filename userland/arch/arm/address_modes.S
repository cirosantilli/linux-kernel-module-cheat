/* https://github.com/cirosantilli/arm-assembly-cheat#addressing-modes */

#include "common.h"

ENTRY

    /* Offset mode with immediate. Add 4 to the address register,
     * which ends up * reading myvar6 instead of myvar.
     */
    adr r4, myvar
    ldr r5, [r4, 4]
    ASSERT_EQ(r5, 0x9ABCDEF0)
    /* r4 was not modified. */
    ASSERT_EQ(r4, myvar)

    /* Pre-indexed mode: modify register, then use it. */
    adr r4, myvar
    ldr r5, [r4, 4]!
    ASSERT_EQ(r5, 0x9ABCDEF0)
    /* r4 was modified. */
    ASSERT_EQ(r4, myvar6)

    /* Post-indexed mode: use register, then modify it. */
    adr r4, myvar
    ldr r5, [r4], 4
    ASSERT_EQ(r5, 0x12345678)
    /* r4 was modified. */
    ASSERT_EQ(r4, myvar6)

    /* Offset in register. */
    adr r4, myvar
    mov r5, 4
    ldr r6, [r4, r5]
    ASSERT_EQ(r6, 0x9ABCDEF0)

    /* Offset in shifted register:
     * r6 =
     * (r4 + (r5 << 1))
     * == *(myvar + (2 << 1))
     * == *(myvar + 4)
     */
    adr r4, myvar
    mov r5, 2
    ldr r6, [r4, r5, lsl 1]
    ASSERT_EQ(r6, 0x9ABCDEF0)

EXIT
myvar:
    .word 0x12345678
myvar6:
    .word 0x9ABCDEF0
