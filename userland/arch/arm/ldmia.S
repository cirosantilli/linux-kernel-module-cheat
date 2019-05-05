/* https://github.com/cirosantilli/arm-assembly-cheat#loop-over-array */

#include "common.h"

#define NELEM 4
#define ELEM_SIZE 4

.data;
my_array_0:
    .word 0x11111111, 0x22222222, 0x33333333, 0x44444444
my_array_1:
    .word 0x55555555, 0x66666666, 0x77777777, 0x88888888

ENTRY

    /* Load r1, r2, r3 and r4 starting from the address in r0. Don't change r0 */
    ldr r0, =my_array_0
    ldr r1, =0
    ldr r2, =0
    ldr r3, =0
    ldr r4, =0
    ldmia r0, {r1-r4}
    ASSERT_EQ(r0, my_array_0)
    ASSERT_EQ(r1, 0x11111111)
    ASSERT_EQ(r2, 0x22222222)
    ASSERT_EQ(r3, 0x33333333)
    ASSERT_EQ(r4, 0x44444444)

    /* Swapping the order of r1 and r2 on the mnemonic makes no difference to load order.
     *
     * But it gives an assembler warning, so we won't do it by default:
     *
     *  ldmia.S: Assembler messages:
     *  ldmia.S:32: Warning: register range not in ascending order
     */
#if 0
    ldr r0, =my_array_0
    ldr r1, =0
    ldr r2, =0
    ldmia r0, {r2,r1}
    ASSERT_EQ(r1, 0x11111111)
    ASSERT_EQ(r2, 0x22222222)
#endif

    /* Modify the array */
    ldr r0, =my_array_1
    ldr r1, =0x55555555
    ldr r2, =0x66666666
    ldr r3, =0x77777777
    ldr r4, =0x88888888
    stmdb r0, {r1-r4}

    /* Verify that my_array_0 changed and is equal to my_array_1. */
    MEMCMP(my_array_0, my_array_1, 0x10)
    ASSERT_EQ(r0, 0)

    /* Load registers and increment r0. */
    ldr r0, =my_array_0
    ldmia r0!, {r1-r4}
    ASSERT_EQ(r0, my_array_1)

EXIT
