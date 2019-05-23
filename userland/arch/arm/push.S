/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-ldmia-instruction */

#include <lkmc.h>

LKMC_PROLOGUE

    /* Save sp before push. */
    mov r4, sp

    /* Push. */
    mov r5, 1
    mov r6, 2
    push {r5, r6}

    /* Save sp after push. */
    mov r5, sp

    /* Restore. */
    mov r7, 0
    mov r8, 0
    pop {r7, r8}
    LKMC_ASSERT_EQ(r7, =1)
    LKMC_ASSERT_EQ(r8, =2)

    /* Check that stack pointer moved down by 8 bytes
     * (2 registers x 4 bytes each).
     */
    sub r4, r5
    LKMC_ASSERT_EQ(r4, =8)

LKMC_EPILOGUE
