.global main
main:
    /* 1 + 2 == 3 */
    mov x0, #1
    /* test-gdb-op1 */
    add x1, x0, #2
    /* test-gdb-result */
    cmp x1, #3
    beq 1f
    bl common_assert_fail
1:
    ret
