/* An example that does not have any CPU pipeline hazards, to observe
 * the simpliest possible steady state of CPU execution.
 * Useful during microarchitectural analysis such as:
 * https://cirosantilli.com/linux-kernel-module-cheat#gem5-event-queue-minorcpu-syscall-emulation-freestanding-example-analysis  
 */

.text
.global _start
_start:
asm_main_after_prologue:

    /* ifetch 1 */
    /* argc */
    ldr x0, [sp]
    mov x1, 1

    /* ifetch 2 */
.Lifetch2:
    mov x2, 2
    mov x3, 3
    mov x4, 4
    mov x5, 5
    mov x6, 6
    /* Check if a CLi argument was passed. */
    cmp x0, 2
    /* Ensure that we will break out on the next loop. */
    mov x0, 3
    blt .Lifetch2

    mov x10, 10
    mov x11, 11
    mov x12, 12
    mov x13, 13
    mov x14, 14
    mov x15, 15
    mov x16, 16
    mov x17, 17

    /* exit */
    mov x0, 0
    mov x8, 93
    svc 0
