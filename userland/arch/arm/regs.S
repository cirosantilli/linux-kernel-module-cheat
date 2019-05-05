/* https://github.com/cirosantilli/arm-assembly-cheat#registers */

#include "common.h"

ENTRY

    /* 13 general purpose registers. */
    mov r0, 0
    mov r1, 1
    mov r2, 2
    mov r3, 3
    mov r4, 4
    mov r5, 5
    mov r6, 6
    mov r7, 7
    mov r8, 8
    mov r9, 9
    mov r10, 10
    mov r11, 11
    mov r12, 12

    /* * r11: aliased to FP (frame pointer, debug stack trace usage only)
     * +
     * I think FP is only a convention with no instruction impact, but TODO:
     * not mentioned on AAPCS. aarch64 AAPCS mentions it though.
     * * r13: aliased to SP (stack pointer), what push / pop use
     * * r14: aliased to LR (link register), what bl writes the return address to
     * * r15: aliased to PC (program counter), contains the current instruction address
     *
     * In ARMv8, SP and PC have dedicated registers in addition to
     * the 32-general purpose ones. LR is still general purpose as before.
     *
     * Therefore, it is possible to use those registers in any place
     * other registers may be used.
     *
     * This is not possible in ARMv8 anymore.
     *
     * For example, we can load an address into PC, which is very similar to what B / BX does:
     * https://stackoverflow.com/questions/32304646/arm-assembly-branch-to-address-inside-register-or-memory/54145818#54145818
     */
    ldr pc, =10f
    FAIL
10:

    /* Same with r15, which is the same as pc. */
    ldr r15, =10f
    FAIL
10:

    /* Another example with mov reading from pc. */
pc_addr:
    mov r0, pc
    /* Why sub 8:
     * https://stackoverflow.com/questions/24091566/why-does-the-arm-pc-register-point-to-the-instruction-after-the-next-one-to-be-e
     */
    sub r0, r0, 8

    /* pc-relative load also just work just like any other register. */
    ldr r0, [pc]
    b 1f
    .word 0x12345678
1:
    ASSERT_EQ(r0, 0x12345678)

    /* We can also use fp in GNU GAS assembly. */
    mov r11, 0
    mov fp, 1
    ASSERT_EQ(r11, 1)
EXIT
