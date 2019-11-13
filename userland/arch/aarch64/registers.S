/* https://cirosantilli.com/linux-kernel-module-cheat#assembly-registers */

#include <lkmc.h>

LKMC_PROLOGUE

    /* 31 64-bit eXtended general purpose registers. */
    mov x0, 0
    mov x1, 1
    mov x2, 2
    mov x3, 3
    mov x4, 4
    mov x5, 5
    mov x6, 6
    mov x7, 7
    mov x8, 8
    mov x9, 9
    mov x10, 10
    mov x11, 11
    mov x12, 12
    mov x13, 13
    mov x14, 14
    mov x15, 15
    mov x16, 16
    mov x17, 17
    mov x18, 18
    mov x19, 19
    mov x20, 20
    mov x21, 21
    mov x22, 22
    mov x23, 23
    mov x24, 24
    mov x25, 25
    mov x26, 26
    mov x27, 27
    mov x28, 28

    /* x29 is used as the frame register by unoptimized GCC: it contains the initial stack.
     * TODO is there a standard recommending it? Does it have effects e.g. on GDB in
     * determining backtraces? */
    mov x29, 29

    /* x30 is the link register. BL stores the return address here. */
    /*mov x30, 30*/

    /* W form addresses the lower 4 bytes word, and zeroes the top. */
    ldr x0, =0x1111222233334444
    ldr x1, =0x5555666677778888
    mov w0, w1
    LKMC_ASSERT_EQ(x0, =0x0000000077778888)
LKMC_EPILOGUE
