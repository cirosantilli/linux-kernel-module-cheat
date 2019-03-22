/* Multiplication. */

#include "common.h"

ENTRY
    /* 2 * 3 = 6 */
    mov r0, 0
    mov r1, 2
    mov r2, 3
    mul r1, r2
    ASSERT_EQ(r1, 6)
EXIT
