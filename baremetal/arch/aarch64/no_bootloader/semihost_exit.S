/* https://cirosantilli.com/linux-kernel-module-cheat#semihosting
 *
 * Since our stack pointer is not setup, we justa allocate a memory
 * region to contain the semihosting arguments, which must be in memory.
 */

.global lkmc_start
lkmc_start:
    mov x1, 0x26
    movk x1, 2, lsl 16
    ldr x2, =.Lsemihost_args
    str x1, [x2, 0]
    mov x0, 0
    str x0, [x2, 8]
    mov x1, x2
    mov w0, 0x18
    hlt 0xf000
.Lsemihost_args:
    .skip 16
