/* https://github.com/cirosantilli/arm-assembly-cheat#bl */

#include "common.h"

ENTRY
    mov r0, 1
    bl inc
    ASSERT_EQ(r0, 2)
EXIT

/* void inc(int *i) { (*i)++ } */
inc:
    add r0, 1
    bx lr
