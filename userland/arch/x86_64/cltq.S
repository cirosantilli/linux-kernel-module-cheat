/* https://cirosantilli.com/linux-kernel-module-cheat#x86-cqto-and-cltq-instructions */

#include <lkmc.h>

LKMC_PROLOGUE
    /* CLTQ: top bit is zero: extend with zeroes. */
    mov $0x123456787FFFFFFF, %rax
    cltq
    LKMC_ASSERT_EQ(%rax, $0x000000007FFFFFFF)

    /* CLTQ: top bit is one: extend with ones. */
    mov $0x1234567880000000, %rax
    cltq
    LKMC_ASSERT_EQ(%rax, $0xFFFFFFFF80000000)

    /* CWTL: zeroes top 32-bits. */
    mov $0x123456789ABC8EF0, %rax
    cwtl
    LKMC_ASSERT_EQ(%rax, $0xFFFF8EF0)
    CWTL

    /* CBTW. */
    mov $0x123456789ABCDE80, %rax
    cbtw
    LKMC_ASSERT_EQ(%rax, $0x123456789ABCFF80)
    CWTL
LKMC_EPILOGUE
