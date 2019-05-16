/* https://github.com/cirosantilli/linux-kernel-module-cheat#magic-failure-string */
.global main
main:
    mov r0, #1
    bx lr
