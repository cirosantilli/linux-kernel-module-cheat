/* https://github.com/cirosantilli/arm-assembly-cheat#ldmia */

#include "common.h"

ENTRY

    /* Save sp before push. */
    mov r0, sp

    /* Push. */
    mov r1, 1
    mov r2, 2
    push {r1, r2}

    /* Save sp after push. */
    mov r1, sp

    /* Restore. */
    mov r3, 0
    mov r4, 0
    pop {r3, r4}
    ASSERT_EQ(r3, 1)
    ASSERT_EQ(r4, 2)

    /* Check that stack pointer moved down by 8 bytes
     * (2 registers x 4 bytes each).
     */
    sub r0, r1
    ASSERT_EQ(r0, 8)

EXIT
