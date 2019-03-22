/* https://github.com/cirosantilli/arm-assembly-cheat#registers */

#include "common.h"

ENTRY
#if 0
    /* Unlike v7, we can't use PC like any other register in ARMv8,
     * since it is not a general purpose register anymore.
     *
     * Only branch instructions can modify the PC.
     *
     * B1.2.1 "Registers in AArch64 state" says:
     *
     * Software cannot write directly to the PC. It
     * can only be updated on a branch, exception entry or
     * exception return.
     */
    ldr pc, =10f
    FAIL
10:
#endif
#if 0
    mov x0, pc
#endif

    /* LDR PC-relative loads exist in ARMv8, but they have a separate encoding
     * "LDR (literal)" instead of "LDR (immediate)":
     * https://stackoverflow.com/questions/28638981/howto-write-pc-relative-adressing-on-arm-asm/54480999#54480999
     */
    ldr x0, pc_relative_ldr
    b 1f
pc_relative_ldr:
    .quad 0x123456789ABCDEF0
1:
    ASSERT_EQ(x0, 0x123456789ABCDEF0)

    /* Just for fun, we can also use relative numbers instead of labels.
     * https://reverseengineering.stackexchange.com/questions/17666/how-does-the-ldr-instruction-work-on-arm/20567#20567
     */
    ldr x0, 0x8
    b 1f
    .quad 0x123456789ABCDEF0
1:
    ASSERT_EQ(x0, 0x123456789ABCDEF0)

    /* Analogous for b with PC. */
    mov x0, 0
    /* Jumps over mov to ASSERT_EQ. */
    b 8
    mov x0, 1
    ASSERT_EQ(x0, 0)

    /* Trying to use the old "LDR (immediate)" PC-relative
     * syntax does not work.
     */
#if 0
    /* 64-bit integer or SP register expected at operand 2 -- `ldr x0,[pc]' */
    ldr x0, [pc]
#endif

    /* There is however no analogue for str. TODO rationale? */
#if 0
    /* Error: invalid addressing mode at operand 2 -- `str x0,pc_relative_str' */
    str x0, pc_relative_str
#endif

    /* You just have to use adr + "STR (register)". */
    ldr x0, pc_relative_str
    ASSERT_EQ(x0, 0x0)
    adr x1, pc_relative_str
    ldr x0, pc_relative_ldr
    str x0, [x1]
    ldr x0, pc_relative_str
    ASSERT_EQ(x0, 0x123456789ABCDEF0)
EXIT
.data
pc_relative_str:
    .quad 0x0000000000000000
