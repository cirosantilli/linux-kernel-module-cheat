/* https://github.com/cirosantilli/arm-assembly-cheat#shift-suffixes */

#include "common.h"

ENTRY

    /* lsr */
    ldr r0, =0xFFF00FFF
    mov r1, r0, lsl 8
    ldr r2, =0xF00FFF00
    ASSERT_EQ_REG(r1, r2)

    /* lsl */
    ldr r0, =0xFFF00FFF
    mov r1, r0, lsr 8
    ldr r2, =0x00FFF00F
    ASSERT_EQ_REG(r1, r2)

    /* ror */
    ldr r0, =0xFFF00FFF
    mov r1, r0, ror 8
    ldr r2, =0xFFFFF00F
    ASSERT_EQ_REG(r1, r2)

    /* asr negative */
    ldr r0, =0x80000008
    mov r1, r0, asr 1
    ldr r2, =0xC0000004
    ASSERT_EQ_REG(r1, r2)

    /* asr positive */
    ldr r0, =0x40000008
    mov r1, r0, asr 1
    ldr r2, =0x20000004
    ASSERT_EQ_REG(r1, r2)

    /* There are also direct shift mnemonics for the mov shifts.
     *
     * They assembly to the exact same bytes as the mov version
     */
    ldr r0, =0xFFF00FFF
    lsl r1, r0, 8
    ldr r2, =0xF00FFF00
    ASSERT_EQ_REG(r1, r2)

    /* If used with the `mov` instruction, it results in a pure shift,
     * but the suffixes also exist for all the other data processing instructions.
     *
     * Here we illustrate a shifted add instruction which calculates:
     *
     * ....
     * r1 = r1 + (r0 << 1)
     * ....
     */
    ldr r0, =0x10
    ldr r1, =0x100
    add r1, r1, r0, lsl 1
    ldr r2, =0x00000120
    ASSERT_EQ_REG(r1, r2)

    /* The shift takes up the same encoding slot as the immediate,
     * therefore it is not possible to both use an immediate and shift.
     *
     * Error: shift expression expected -- `add r1,r0,1,lsl#1'
     */
#if 0
    add r1, r0, 1, lsl 1
#endif

    /* However, you can still encode shifted bitmasks of
     * limited width in immediates, so why not just use the
     * assembler pre-processing for it?
     */
    ldr r1, =0x100
    add r1, r1, (0x10 << 1)
    ldr r2, =0x00000120
    ASSERT_EQ_REG(r1, r2)

EXIT
