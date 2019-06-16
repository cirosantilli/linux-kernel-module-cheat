/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-bit-and-byte-instructions */

#include <lkmc.h>

LKMC_PROLOGUE
    /* 0xF0 & 0x00 == 0x00 */
    mov $0xF0, %r12
    test $0, %r12b
    /* The comparison was equal 0. */
    LKMC_ASSERT(jz)
    /* r12 is unchanged. */
    LKMC_ASSERT_EQ(%r12, $0x0F0)

    /* 0xF0 & 0x18 == 0x10 != 0x00 */
    mov $0xF0, %r12
    test $0x18, %r12b
    LKMC_ASSERT(jnz)
    LKMC_ASSERT_EQ(%r12, $0x0F0)

    /* test %rax, %rax vs cmp $0, %rax: test produces a shorter
     * encoding to decide if a register equals zero or not.
     * http://stackoverflow.com/questions/147173/x86-assembly-testl-eax-against-eax
     */
LKMC_EPILOGUE
