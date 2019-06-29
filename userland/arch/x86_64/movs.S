/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-string-instructions */

#include <lkmc.h>

.section .rodata
    src: .quad 1, 2
.bss
    dest: .skip 16
LKMC_PROLOGUE
    cld
    lea src(%rip), %rsi
    lea dest(%rip), %rdi
    movsq
    movsq
    LKMC_ASSERT_EQ(dest + 0, $1)
    LKMC_ASSERT_EQ(dest + 8, $2)
LKMC_EPILOGUE
