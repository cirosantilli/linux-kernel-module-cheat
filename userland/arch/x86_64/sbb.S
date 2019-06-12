/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-binary-arithmetic-instructions
 *
 * Subtract with Borrow. Like ADC is for ADD, but for subtraction.
 */

#include <lkmc.h>

LKMC_PROLOGUE
    /*  rax :                rbx -=  rcx :                rdx
     *    1 :                  0 -=    0 : 0x8000000000000000
     *    0 : 0x8000000000000000
     */
    mov $0x1, %rax
    mov $0x0, %rbx
    mov $0x0, %rcx
    mov $0x8000000000000000, %rdx
    sub %rdx, %rbx
    sbb %rcx, %rax
    mov %rax, %r12
    mov %rbx, %r13
    LKMC_ASSERT_EQ(%r12, $0x0)
    LKMC_ASSERT_EQ(%r13, $0x8000000000000000)
LKMC_EPILOGUE
