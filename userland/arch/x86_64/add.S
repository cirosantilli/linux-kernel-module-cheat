/* https://github.com/cirosantilli/linux-kernel-module-cheat#userland-assembly */

#include <lkmc.h>

LKMC_PROLOGUE
    mov $1, %rax
    add $2, %rax
    LKMC_ASSERT_EQ(%rax, $3)
LKMC_EPILOGUE
