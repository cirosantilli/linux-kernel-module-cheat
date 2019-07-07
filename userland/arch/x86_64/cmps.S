/* https://cirosantilli.com/linux-kernel-module-cheat#x86-string-instructions */

#include <lkmc.h>

.section .rodata
    my_quad_array_1: .quad 1, 2
    my_quad_array_2: .quad 1, 3
LKMC_PROLOGUE
    mov $0, %r12
    mov $0, %r13
    cld
    lea my_quad_array_1(%rip), %rsi
    lea my_quad_array_2(%rip), %rdi
    cmpsq
    setz %r12b
    cmpsq
    setz %r13b
    /* 1 == 1 */
    LKMC_ASSERT_EQ(%r12, $1)
    /* 2 != 3 */
    LKMC_ASSERT_EQ(%r13, $0)
LKMC_EPILOGUE
