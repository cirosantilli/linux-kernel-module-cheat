/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-exchange-instructions */

#include <lkmc.h>

LKMC_PROLOGUE
    mov $0, %rax
    mov $1, %rbx

    xchg %rbx, %rax
    LKMC_ASSERT_EQ(%rax, $1)
    LKMC_ASSERT_EQ(%rbx, $0)

    xchg %rbx, %rax
    LKMC_ASSERT_EQ(%rax, $0)
    LKMC_ASSERT_EQ(%rbx, $1)
LKMC_EPILOGUE
