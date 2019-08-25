#include <lkmc.h>

/* This is implemented in assembly so that it does not use the stack,
 * and thus can be called safely from programs without the bootloader.
 * C signature:
 *
 * void _exit(int status)
 *
 * If only there was a GCC attribute to create such a function!
 */
.text
.global _exit
_exit:
#if LKMC_GEM5
    LKMC_M5OPS_EXIT_ASM
#else
/* Use semihosting:
 * https://cirosantilli.com/linux-kernel-module-cheat#semihosting */
#if defined(__arm__)
    mov r0, #0x18
    ldr r1, =#0x20026
    svc 0x00123456
#elif defined(__aarch64__)
    mov x1, 0x26
    movk x1, 2, lsl 16
    ldr x2, =.Lsemihost_args
    str x1, [x2, 0]
    str x0, [x2, 8]
    mov x1, x2
    mov w0, 0x18
    hlt 0xf000
.Lsemihost_args:
    .skip 16
#endif
#endif
