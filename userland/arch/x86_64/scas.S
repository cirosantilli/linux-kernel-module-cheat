/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-string-instructions */

#include <lkmc.h>

.section .rodata
    my_quad_array: .quad 1, 2
LKMC_PROLOGUE
    mov $0, %r12
    mov $0, %r13
    /* RDI holds the address. */
    lea my_quad_array(%rip), %rdi
    cld
    mov $1, %rax
    /* Compare RAX to *RDI (1 == 1)  */
    scasq
    setz %r12b
    mov $3, %rax
    /* Compare RAX to *RDI (3 == 2)  */
    scasq
    setz %r13b
    /* 1 == 1 */
    LKMC_ASSERT_EQ(%r12, $1)
    /* 2 != 3 */
    LKMC_ASSERT_EQ(%r13, $0)
LKMC_EPILOGUE
