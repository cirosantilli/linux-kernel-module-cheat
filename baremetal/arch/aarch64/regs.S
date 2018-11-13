/* Test that we can move:
 * - set registers
 * - read x30 */
.global main
main:
    mov x0, #1
    /* test-gdb-x0 */
    mov x1, #2
    /* test-gdb-x1 */

    mov x29, #1
    /* test-gdb-x29 */
    mov x30, #2
    /* test-gdb-x30 */

    /* Exit required since we meesed up with x30 which is the lr. */
    mov x0, #0
    bl exit
