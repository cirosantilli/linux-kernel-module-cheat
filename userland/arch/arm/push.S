/* https://github.com/cirosantilli/arm-assembly-cheat#ldmia */

#include "common.h"

ENTRY

    /* Save sp before push. */
    mov r4, sp

    /* Push. */
    mov r5, 1
    mov r6, 2
    push {r5, r6}

    /* Save sp after push. */
    mov r5, sp

    /* Restore. */
    mov r7, 0
    mov r8, 0
    pop {r7, r8}
    ASSERT_EQ(r7, 1)
    ASSERT_EQ(r8, 2)

    /* Check that stack pointer moved down by 8 bytes
     * (2 registers x 4 bytes each).
     */
    sub r4, r5
    ASSERT_EQ(r4, 8)

EXIT
