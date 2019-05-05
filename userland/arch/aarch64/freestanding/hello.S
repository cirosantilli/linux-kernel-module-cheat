/* https://github.com/cirosantilli/arm-assembly-cheat#linux-system-calls */

.text
.global _start
_start:
asm_main_after_prologue:
    /* write */
    mov x0, 1     /* stdout */
    adr x1, msg   /* buffer */
    ldr x2, =len  /* len */
    mov x8, 64    /* syscall number */
    svc 0

    /* exit */
    mov x0, 0     /* exit status */
    mov x8, 93    /* syscall number */
    svc 0
msg:
    .ascii "hello\n"
len = . - msg
