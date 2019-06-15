/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-jcc-instructions */

#include <lkmc.h>

LKMC_PROLOGUE
    /* JZ, JE */
    mov $1, %r12
    cmp $1, %r12
    LKMC_ASSERT(jz)
    cmp $2, %r12
    LKMC_ASSERT(jnz)

    /* JA, JB, JG, JL */

        /* 0x0 ==
         *
         * * 0 in 2's complement signed
         * * 0 in 2's complement unsigned
         */
        mov $0, %al

        /* 0xFF ==
         *
         * *  -1 in 2's complement signed
         * * 255 in 2's complement unsigned
         */
        mov $0xFF, %bl

        /* Do the operation. */
        cmp %bl, %al

        /* We push and pop flags with PUSHF and POPF because
         * our assert function might change them. All comparisons
         * are done with the flags of the original cmp operation.
         *
         * We push twice to keep the stack aligned to 16 bits
         * when calling our C asset function.
         */
        pushf
        pushf

        /* 0 < 255 */
        LKMC_ASSERT(jb)
        popf
        pushf

        /* !(0 > 255) */
        LKMC_ASSERT(jna)
        popf
        pushf

        /* !(0 < -1) */
        LKMC_ASSERT(jnl)
        popf
        pushf

        /* 0 > -1 */
        LKMC_ASSERT(jg)

        /* Restore stack. */
        add $16, %rsp
LKMC_EPILOGUE
