/* arm freestanding Linux hello world
 * https://github.com/cirosantilli/linux-kernel-module-cheat#linux-system-calls
 */

.syntax unified
.text
.global _start
_start:
asm_main_after_prologue:
    /* write */
    mov r0, 1     /* stdout */
    adr r1, msg   /* buffer */
    ldr r2, =len  /* len */
    mov r7, 4     /* syscall number */
    svc 0

    /* exit */
    mov r0, 0     /* exit status */
    mov r7, 1     /* syscall number */
    svc 0
msg:
    .ascii "hello\n"
len = . - msg
