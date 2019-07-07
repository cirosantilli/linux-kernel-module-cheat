/* https://cirosantilli.com/linux-kernel-module-cheat#x86-exchange-instructions */

#include <lkmc.h>

LKMC_PROLOGUE
    mov $1, %rax
    mov $2, %rbx
    xadd %rbx, %rax
    LKMC_ASSERT_EQ(%rax, $3)
    LKMC_ASSERT_EQ(%rbx, $1)
LKMC_EPILOGUE
