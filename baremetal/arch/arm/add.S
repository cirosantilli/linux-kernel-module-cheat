.global main
main:
    /* 1 + 2 == 3 */
    mov r0, #1
    /* test-gdb-op1 */
    add r1, r0, #2
    /* test-gdb-result */
    cmp r1, #3
    beq 1f
    bl assert_fail
1:
    bx lr
