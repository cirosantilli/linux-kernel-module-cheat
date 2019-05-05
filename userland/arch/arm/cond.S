/* https://github.com/cirosantilli/arm-assembly-cheat#conditional-execution */

#include "common.h"

ENTRY
    mov r0, 0
    mov r1, 1
    cmp r0, 1
    /* Previous cmp failed, skip this operation. */
    addeq r1, 1
    ASSERT_EQ(r1, 1)
    cmp r0, 0
    /* Previous passed, do this operation. */
    addeq r1, 1
    ASSERT_EQ(r1, 2)
EXIT
