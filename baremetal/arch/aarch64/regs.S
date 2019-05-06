/* Test that we can move:
 * - set registers
 * - read x30
 */
.global main
main:
    mov x0, 1
    /* test-gdb-x0 */
    mov x1, 2
    /* test-gdb-x1 */

    mov x29, 1
    /* test-gdb-x29 */
    mov x30, 2
    /* test-gdb-x30 */

    fmov d0, 1.5
    /* test-gdb-d0 */
    fmov d1, 2.5
    /* test-gdb-d1 */

    fmov d30, 1.5
    /* test-gdb-d30 */
    fmov d31, 2.5
    /* test-gdb-d31 */

    /* Exit required since we messed up with x30 which is the lr. */
    mov x0, 0
    bl exit
