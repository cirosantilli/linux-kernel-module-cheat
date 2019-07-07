 /* https://cirosantilli.com/linux-kernel-module-cheat#linux-system-calls
  *
  * int $0x80 sycalls are still supported by x86_64 for some kind of backwards compatibility,
  * (TODO so when x86_64 started it didn't have SYSCALL?) althoug you should prefre
  * SYSCALL / VSDO.
  *
  * https://stackoverflow.com/questions/29440225/in-linux-x86-64-are-syscalls-and-int-0x80-related
  */

.text
.global _start
_start:
    /* write */
    mov $4, %rax
    mov $1, %rbx
    lea msg(%rip), %rcx
    mov $len, %rdx
    int $0x80

    /* exit */
    mov $1, %rax
    mov $0, %rbx
    int $0x80
msg:
    .ascii "hello\n"
    len = . - msg
