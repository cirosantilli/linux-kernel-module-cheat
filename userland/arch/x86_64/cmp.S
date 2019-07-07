/* https://cirosantilli.com/linux-kernel-module-cheat#userland-assembly
 *
 * Compare two numbers and set the flags register.
 *
 * `cmp X, Y` does `X - Y` and ignores the exact result, but sets
 * all flags that would be set on the subtraction, just like SUB does.
 */

#include <lkmc.h>

LKMC_PROLOGUE
    /* 2 == 2 */
    mov $2, %rax
    cmp $2, %rax
    LKMC_ASSERT(je)

    /* 2 > 1 */
    mov $2, %rax
    cmp $1, %rax
    LKMC_ASSERT(ja)
LKMC_EPILOGUE
