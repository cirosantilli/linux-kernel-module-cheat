/* https://github.com/cirosantilli/arm-assembly-cheat#calling-convention */

#include "common.h"

.data
puts_s:
    .asciz "hello puts"
printf_format:
    .asciz "hello printf %x\n"
my_array_0:
    .word 0x11111111, 0x22222222, 0x33333333, 0x44444444
my_array_1:
    .word 0x55555555, 0x66666666, 0x77777777, 0x88888888

ENTRY
    /* puts("hello world") */
    /* r0 is first argument. */
    ldr r0, =puts_s
    bl puts
    /* Check exit statut >= 0 for success. */
    cmp r0, 0
    ASSERT(bge)

    /* printf */
    ldr r0, =printf_format
    ldr r1, =0x12345678
    bl printf
    cmp r0, 0
    ASSERT(bge)

    /* memcpy and memcmp. */

        /* Smaller. */
        ldr r0, =my_array_0
        ldr r1, =my_array_1
        ldr r2, =0x10
        bl memcmp
        cmp r0, 0
        ASSERT(blt)

        /* Copy. */
        ldr r0, =my_array_0
        ldr r1, =my_array_1
        ldr r2, =0x10
        bl memcpy

        /* Equal. */
        ldr r0, =my_array_0
        ldr r1, =my_array_1
        ldr r2, =0x10
        bl memcmp
        ASSERT_EQ(r0, 0)

    /* exit(0) */
    mov r0, 0
    bl exit

    /* Never reached, just for the fail symbol. */
EXIT
