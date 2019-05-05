/* https://github.com/cirosantilli/arm-assembly-cheat#s-suffix */

#include "common.h"

ENTRY

    /* Result is 0, set beq. */
    movs r0, 0
    ASSERT(beq)

    /* The opposite. */
    movs r0, 1
    ASSERT(bne)

    /* mov without s does not set the status. */
    movs r0, 0
    mov r0, 1
    ASSERT(beq)

    /* movs still moves... */
    mov r0, 0
    movs r0, 1
    ASSERT_EQ(r0, 1)

    /* add: the result is 0. */
    mov r0, 1
    adds r0, -1
    ASSERT(beq)

    /* add: result non 0. */
    mov r0, 1
    adds r0, 1
    ASSERT(bne)

EXIT
