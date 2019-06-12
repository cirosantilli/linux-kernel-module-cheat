/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-logical-instructions */

#include <lkmc.h>

LKMC_PROLOGUE
    mov $0x00FF, %rax
    and $0x0F0F, %rax
    LKMC_ASSERT_EQ(%rax, $0x000F)
LKMC_EPILOGUE
