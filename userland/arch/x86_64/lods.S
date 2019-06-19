/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-string-instructions */

#include <lkmc.h>

.section .rodata
    my_quad_array: .quad 1, 2
LKMC_PROLOGUE
    lea my_quad_array(%rip), %rsi
    cld
    lodsq
    mov %rax, %r12
    lodsq
    mov %rax, %r13
    LKMC_ASSERT_EQ(%r12, $1)
    LKMC_ASSERT_EQ(%r13, $2)
LKMC_EPILOGUE
