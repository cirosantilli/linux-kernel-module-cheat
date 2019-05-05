/* https://github.com/cirosantilli/arm-assembly-cheat#loop-over-array */

#include "common.h"

#define NELEM 4
#define ELEM_SIZE 4

.data;
my_array:
    .word 0x11111111, 0x22222222, 0x33333333, 0x44444444
my_array_expect:
    .word 0x11111112, 0x22222223, 0x33333334, 0x44444445

ENTRY
    /* Increment. */
    ldr r0, =my_array
    mov r1, NELEM
increment:
    ldr r2, [r0]
    add r2, 1
    /* Post index usage. */
    str r2, [r0], ELEM_SIZE
    sub r1, 1
    cmp r1, 0
    bne increment
    ASSERT_MEMCMP(my_array, my_array_expect, 0x10)
EXIT
