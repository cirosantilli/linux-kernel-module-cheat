/* https://cirosantilli.com/linux-kernel-module-cheat#x86-bit-and-byte-instructions */

#include <lkmc.h>

LKMC_PROLOGUE
    mov $0, %rbx
    popcnt %rbx, %rax
    LKMC_ASSERT_EQ(%rax, $0)

    mov $1, %rbx
    popcnt %rbx, %rax
    LKMC_ASSERT_EQ(%rax, $1)

    mov $2, %rbx
    popcnt %rbx, %rax
    LKMC_ASSERT_EQ(%rax, $1)

    mov $3, %rbx
    popcnt %rbx, %rax
    LKMC_ASSERT_EQ(%rax, $2)

    mov $4, %rbx
    popcnt %rbx, %rax
    LKMC_ASSERT_EQ(%rax, $1)

    mov $5, %rbx
    popcnt %rbx, %rax
    LKMC_ASSERT_EQ(%rax, $2)

    mov $6, %rbx
    popcnt %rbx, %rax
    LKMC_ASSERT_EQ(%rax, $2)

    mov $7, %rbx
    popcnt %rbx, %rax
    LKMC_ASSERT_EQ(%rax, $3)
LKMC_EPILOGUE
