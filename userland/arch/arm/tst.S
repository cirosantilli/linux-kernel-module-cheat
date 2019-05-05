/* Test. Same as ands, but don't store the result, just update flags. */

#include "common.h"

ENTRY

    /* 0x0F && 0xF0 == 0x00, so beq. */
    mov r0, 0x0F
    tst r0, 0xF0
    ASSERT(beq)

    /* bne */
    mov r0, 0xFF
    tst r0, 0x0F
    ASSERT(bne)
    # r0 was not modified.
    ASSERT_EQ(r0, 0xFF)

EXIT
