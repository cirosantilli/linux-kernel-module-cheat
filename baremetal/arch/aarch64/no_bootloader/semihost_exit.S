/* https://github.com/cirosantilli/linux-kernel-module-cheat#semihosting */

.global mystart
mystart:
    mov x1, #0x26
    movk x1, #2, lsl #16
    str x1, [sp,#0]
    mov x0, #0
    str x0, [sp,#8]
    mov x1, sp
    mov w0, #0x18
    hlt 0xf000
