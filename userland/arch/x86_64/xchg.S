/* https://cirosantilli.com/linux-kernel-module-cheat#x86-exchange-instructions */

#include <lkmc.h>

LKMC_PROLOGUE
    mov $0, %r12
    mov $1, %r13

    xchg %r13, %r12
    LKMC_ASSERT_EQ(%r12, $1)
    LKMC_ASSERT_EQ(%r13, $0)

    xchg %r13, %r12
    LKMC_ASSERT_EQ(%r12, $0)
    LKMC_ASSERT_EQ(%r13, $1)
LKMC_EPILOGUE
