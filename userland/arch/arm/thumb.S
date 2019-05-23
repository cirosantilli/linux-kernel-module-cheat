/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-instruction-encodings
 *
 * Illustrates features that are only available in thumb.
 * TODO ensure that we are actually inside of thumb.
 */

.syntax unified
.text
.thumb_func
.global main
main:
main_after_prologue:

    /* CBZ: cmp and branch if zero instruction. Equivalent to CMP + BEQ.
     * TODO create an interesting assertion here.
     */
    cbz r1, 1f
    1:

    mov r0, 0
    bx lr
