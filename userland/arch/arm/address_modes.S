/* https://github.com/cirosantilli/arm-assembly-cheat#addressing-modes */

#include "common.h"

ENTRY

    /* Offset mode with immediate. Add 4 to the address register, which ends up
     * reading myvar2 instead of myvar.
     */
    adr r0, myvar
    ldr r1, [r0, 4]
    ASSERT_EQ(r1, 0x9ABCDEF0)
    /* r0 was not modified. */
    ASSERT_EQ(r0, myvar)

    /* Pre-indexed mode */
    adr r0, myvar
    ldr r1, [r0, 4]!
    ASSERT_EQ(r1, 0x9ABCDEF0)
    /* r0 was modified. */
    ASSERT_EQ(r0, myvar2)

    /* Post-indexed mode */
    adr r0, myvar
    ldr r1, [r0], 4
    ASSERT_EQ(r1, 0x12345678)
    /* r0 was modified. */
    ASSERT_EQ(r0, myvar2)

    /* Offset in register. */
    adr r0, myvar
    mov r1, 4
    ldr r2, [r0, r1]
    ASSERT_EQ(r2, 0x9ABCDEF0)

    /* Offset in shifted register:
     * r2 =
     * (r0 + (r1 << 1))
     * == *(myvar + (2 << 1))
     * == *(myvar + 4)
     */
    adr r0, myvar
    mov r1, 2
    ldr r2, [r0, r1, lsl 1]
    ASSERT_EQ(r2, 0x9ABCDEF0)

EXIT
myvar:
    .word 0x12345678
myvar2:
    .word 0x9ABCDEF0
