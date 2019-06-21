/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-data-transfer-instructions
 *
 * mov and zero extend
 *
 * unsigned typecast to larger types in 2's complement.
 *
 * MOV does this automatically from 32 to 64 bits:
 * https://stackoverflow.com/questions/11177137/why-do-x86-64-instructions-on-32-bit-registers-zero-the-upper-part-of-the-full-6
 */

#include <lkmc.h>

LKMC_PROLOGUE
    /* Top bit is 0, extend with zero. */
    mov $0x1234567F, %eax
    movzx %al, %ax
    LKMC_ASSERT_EQ_32(%eax, $0x1234007F)

    /* Top bit is 1: does not matter, stil zero extends. */
    mov $0x1234568F, %eax
    movzx %al, %ax
    LKMC_ASSERT_EQ_32(%eax, $0x1234008F)

#if 0
    /* must be a register, otherwise x86 cannot know how to size it:
     * Error: unsupported syntax for `movzx' */
    movzx $0, %eax

    /* Operands have the same size. Fist must be larger.
     * Error: unsupported syntax for `movzx' */
    movzx %al, %al
#endif
LKMC_EPILOGUE
