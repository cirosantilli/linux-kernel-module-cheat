/* See the aarch64 version. */
.global main
main:
    mov r0, #1
    /* test-gdb-r0 */
    mov r1, #2
    /* test-gdb-r1 */
    mov r0, #0
    bx lr
