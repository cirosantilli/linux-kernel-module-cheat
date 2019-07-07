/* https://cirosantilli.com/linux-kernel-module-cheat#x86-data-transfer-instructions */

#include <lkmc.h>

LKMC_PROLOGUE
    /* First example. */

        /* Clear carry flag. */
        clc
        /* Save RSP before PUSHF. */
        mov %rsp, %r12
        pushf
        /* Save stack value after PUSHF. Should contain the original FLAGS. */
        mov (%rsp), %r13
        /* Re-align stack to 16-bits for our asserts. */
        sub $8, %rsp

        /* The stack went down by 16: 8 from PUSHF, 8 from our SUB. */
        sub %rsp, %r12
        LKMC_ASSERT_EQ(%r12, $16)

        /* Check that bit 0 (Carry Flag) of R13 is clear. */
        bt $0, %r13
        LKMC_ASSERT(jnc)

        /* Restore the stack. */
        add $16, %rsp

    /* Now let's set carry flag instead. */
    stc
    pushf
    mov (%rsp), %r13
    sub $8, %rsp
    bt $0, %r13
    /* Assert that it was pushed to stack set. */
    LKMC_ASSERT(jc)
    add $16, %rsp

    /* POPF pops the stack into flags of course. */
    clc
    pushf
    stc
    popf
    LKMC_ASSERT(jnc)

    /* PUSHFQ has the same opcode as PUSHF in the Intel manual.
     * which mentions that PUSHF can be requested with a prefix.
     *
     * GNU GAS 2.32 emits the same PUSHFQ code for both by default.
     * according to objdump.
     */
    clc
    pushf
    stc
    popf
    LKMC_ASSERT(jnc)
LKMC_EPILOGUE
