/* https://cirosantilli.com/linux-kernel-module-cheat#arm-conditional-execution */

#include <lkmc.h>

LKMC_PROLOGUE
    mov r4, 0
    mov r5, 1

    /* Previous cmp failed, skip this operation. */
    cmp r4, 1
    addeq r5, 1
    LKMC_ASSERT_EQ(r5, =1)

    /* Previous passed, do this operation. */
    cmp r4, 0
    addeq r5, 1
    LKMC_ASSERT_EQ(r5, =2)
LKMC_EPILOGUE
