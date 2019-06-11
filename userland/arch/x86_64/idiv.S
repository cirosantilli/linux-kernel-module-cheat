/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-binary-arithmetic-instructions
 *
 * Signed integer division.
 *
 * Much like MUL vs IMUL.
 */

#include <lkmc.h>

LKMC_PROLOGUE
    movl $-5, %eax
    # Don't forget this!
    cltd
    movl $-2, %ecx
    idivl %ecx
    LKMC_ASSERT_EQ_32(%eax, $2)
    LKMC_ASSERT_EQ_32 edx, -1

    movl $1, %eax
    movl $1, %edx
    movl $4, %ecx
    idivl %ecx
    LKMC_ASSERT_EQ_32(%eax, $0x40000000)
    LKMC_ASSERT_EQ_32(%edx, $1)

    # RUNTIME ERROR: result must fit into signed dword:
    #mov eax, 1
    #mov edx, 1
    #mov ecx, 2
    #idiv ecx

    # TODO division by zero

LKMC_EPILOGUE
