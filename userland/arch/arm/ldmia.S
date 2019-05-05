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

    /* Load r5, r6, r7 and r8 starting from the address in r4. Don't change r4 */
    ldr r4, =my_array_0
    ldr r5, =0
    ldr r6, =0
    ldr r7, =0
    ldr r8, =0
    ldmia r4, {r5-r8}
    ASSERT_EQ(r4, my_array_0)
    ASSERT_EQ(r5, 0x11111111)
    ASSERT_EQ(r6, 0x22222222)
    ASSERT_EQ(r7, 0x33333333)
    ASSERT_EQ(r8, 0x44444444)

    /* Swapping the order of r5 and r6 on the mnemonic makes no difference to load order.
     *
     * But it gives an assembler warning, so we won't do it by default:
     *
     *  ldmia.S: Assembler messages:
     *  ldmia.S:32: Warning: register range not in ascending order
     */
#if 0
    ldr r4, =my_array_0
    ldr r5, =0
    ldr r6, =0
    ldmia r4, {r6,r5}
    ASSERT_EQ(r5, 0x11111111)
    ASSERT_EQ(r6, 0x22222222)
#endif

    /* Modify the array */
    ldr r4, =my_array_1
    ldr r5, =0x55555555
    ldr r6, =0x66666666
    ldr r7, =0x77777777
    ldr r8, =0x88888888
    stmdb r4, {r5-r8}

    /* Verify that my_array_0 changed and is equal to my_array_1. */
    ASSERT_MEMCMP(my_array_0, my_array_1, 0x10)

    /* Load registers and increment r4. */
    ldr r4, =my_array_0
    ldmia r4!, {r5-r8}
    ASSERT_EQ(r4, my_array_1)

EXIT
