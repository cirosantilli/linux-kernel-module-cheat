/* https://github.com/cirosantilli/linux-kernel-module-cheat#semihosting */

.global mystart
mystart:
    mov x1, 0x26
    movk x1, 2, lsl 16
    ldr x2, =semihost_args
    str x1, [x2, 0]
    mov x0, #0
    str x0, [x2, 8]
    mov x1, x2
    mov w0, 0x18
    hlt 0xf000
semihost_args:
    .skip 16
