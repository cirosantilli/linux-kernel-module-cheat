/* Minimal example using driver.
 *
 * Controls the exit status of the program.
 */

.syntax unified
.text
.global asm_main
asm_main:
asm_main_after_prologue:

    /* Set the return value according to the ARM calling convention. */
    mov r0, 0

    /* Try some whacky value to see tests break. */
    /*mov r0, 77*/

    /* Branch to the address at register lr.
     * That is the return value which was put there by the C driver (likely with a bl).
     *
     * X means eXchange encoding from thumb back to ARM, which is what the driver uses.
     */
    bx lr
