/* https://cirosantilli.com/linux-kernel-module-cheat#x86-logical-instructions */

#include <lkmc.h>

LKMC_PROLOGUE
    mov $0x0FF0, %rax
    not %ax
    LKMC_ASSERT_EQ(%rax, $0xF00F)
LKMC_EPILOGUE
