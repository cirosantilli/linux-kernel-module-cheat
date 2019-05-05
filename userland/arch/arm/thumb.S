/* Illustrates features that are only available in thumb. */

.syntax unified
.text
.thumb_func
.global asm_main
asm_main:
asm_main_after_prologue:

    /* CBZ: cmp and branch if zero instruction. Equivalent to CMP + BEQ.
     * TODO create an interesting assertion here.
     */
    cbz r1, 1f
    1:

    mov r0, 0
    bx lr
