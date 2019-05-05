/* x86_64 freestanding Linux hello world
 * https://github.com/cirosantilli/linux-kernel-module-cheat#linux-system-calls
 */

.text
.global _start
_start:
asm_main_after_prologue:
    /* write */
    mov $1, %rax    /* syscall number */
    mov $1, %rdi    /* stdout */
    mov $msg, %rsi  /* buffer */
    mov $len, %rdx  /* len */
    syscall

    /* exit */
    mov $60, %rax   /* syscall number */
    mov $0, %rdi    /* exit status */
    syscall
msg:
    .ascii "hello\n"
len = . - msg
