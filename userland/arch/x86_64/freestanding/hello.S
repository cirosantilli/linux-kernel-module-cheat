.data
    s:
        .ascii "hello\n"
        len = . - s
.text
    .global _start
    _start:

    /* Write. */
    mov $1, %rax
    mov $1, %rdi
    mov $s, %rsi
    mov $len, %rdx
    syscall

    /* Exit. */
    mov $60, %rax
    mov $0, %rdi
    syscall
