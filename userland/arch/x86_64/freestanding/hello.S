.text
.global _start
_start:
asm_main_after_prologue:
    /* write */
    mov $1, %rax    /* stdout */
    mov $1, %rdi    /* buffer */
    mov $msg, %rsi  /* len */
    mov $len, %rdx  /* syscall number */
    syscall

    /* exit */
    mov $60, %rax   /* exit status */
    mov $0, %rdi    /* syscall number */
    syscall
msg:
    .ascii "hello\n"
len = . - msg
