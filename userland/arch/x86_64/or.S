/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-logical-instructions */

#include <lkmc.h>

LKMC_PROLOGUE
    mov $0x00FF, %rax
    or $0x0F0F, %ax
    LKMC_ASSERT_EQ(%rax, $0x0FFF)
LKMC_EPILOGUE
