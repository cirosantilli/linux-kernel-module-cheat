/* https://cirosantilli.com/linux-kernel-module-cheat#x86-binary-arithmetic-instructions
 *
 * Add with Carry. Like add, but if the carry flag is set, add 1 to the addition.
 *
 * This allows implementing arbitrary precision arithmetic.
 */

#include <lkmc.h>

LKMC_PROLOGUE
    /*  rax :                rbx +=  rcx :                rdx
     *    1 : 0x8000000000000001 += 0x10 : 0x8000000000000010
     * 0x12 :               0x11
     */
    mov $0x1, %rax
    mov $0x8000000000000001, %rbx
    mov $0x10, %rcx
    mov $0x8000000000000010, %rdx
    add %rdx, %rbx
    adc %rcx, %rax
    mov %rax, %r12
    mov %rbx, %r13
    LKMC_ASSERT_EQ(%r12, $0x12)
    LKMC_ASSERT_EQ(%r13, $0x11)
LKMC_EPILOGUE
