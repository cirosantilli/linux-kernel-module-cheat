/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-string-instructions */

#include <lkmc.h>

.data
    my_quad_array: .quad 0, 0
    my_quad_array_expect_forward: .quad 1, 2
    my_quad_array_expect_backwards: .quad 4, 3
LKMC_PROLOGUE

    /* Clear the direction flag: move forward. */
    cld

    /* The target address is stored in RDI. */
    lea my_quad_array(%rip), %rdi

    /* my_quad_array[0] = 1 */
    mov $1, %rax
    /* RAX is automatically used as the source. */
    stosq

    /* my_quad_array[1] = 2 */
    mov $2, %rax
    stosq

    /* RDI moved 2x 8 bytes forward. */
    sub $my_quad_array, %rdi
    LKMC_ASSERT_EQ(%rdi, $0x10)

    /* The memory was modified. */
    LKMC_ASSERT_MEMCMP(
        my_quad_array,
        my_quad_array_expect_forward,
        $0x10
    )

    /* Now with backwards direction. */
    std

    /* The target address is stored in RDI. */
    lea (my_quad_array + 8)(%rip), %rdi

    /* my_quad_array[1] = 3 */
    mov $3, %rax
    stosq

    /* my_quad_array[0] = 4 */
    mov $4, %rax
    stosq

    /* RDI moved 2x 8 bytes backwards. */
    sub $my_quad_array, %rdi
    LKMC_ASSERT_EQ(%rdi, $-0x8)

    /* The memory was modified. */
    LKMC_ASSERT_MEMCMP(
        my_quad_array,
        my_quad_array_expect_backwards,
        $0x10
    )

LKMC_EPILOGUE
