/* assert 0x12345678 + 1 == 0x12345679 */
.global main
main:
    movw r0, #:lower16:myvar
    movt r0, #:upper16:myvar
    ldr r1, [r0]
    add r1, r1, #1
    str r1, [r0]
    movw r2, #0x5679
    movt r2, #0x1234
    cmp r1, r2
    beq ok
    /* m5 fail 1 */
    mov r0, #0; mov r1, #0; mov r2, #1; mov r3, #0; .inst 0xEE000110 | (0x22 << 16);
ok:
    /* m5 exit */
    mov r0, #0; mov r1, #0; .inst 0xEE000110 | (0x21 << 16);
myvar:
    .word 0x12345678
