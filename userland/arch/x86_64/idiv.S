/* https://cirosantilli.com/linux-kernel-module-cheat#x86-binary-arithmetic-instructions
 *
 * Signed integer division.
 */

#include <lkmc.h>

LKMC_PROLOGUE
    /* Without operands, it works like DIV.
     * -5 = (2 * -2) + (-1)
     */
    mov $-5, %rax
    /* Sign extend rax into rdx:rax
     * https://stackoverflow.com/questions/17170388/trying-to-understand-the-assembly-instruction-cltd-on-x86/50315201#50315201
     */
    cqo
    mov $2, %rbx
    idiv %rbx
    mov %rax, %r12
    mov %rdx, %r13
    LKMC_ASSERT_EQ(%r12, $-2)
    LKMC_ASSERT_EQ(%r13, $-1)

#if 0
    /* Unlike IMUL vs MUL, IDIV does not have a multi operand interface.
     * Likely because it need 2 output registers unlike IMUL.
     *
     * ....
     * Error: number of operands mismatch for `idiv'
     * ....
     */
    idiv %rax, $2, %rbx
#endif
LKMC_EPILOGUE
