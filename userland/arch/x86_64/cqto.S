/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-cqto-and-cltq-instructions */

#include <lkmc.h>

LKMC_PROLOGUE
    /* Quad to Octo: top bit is zero: extend with zeroes. */
    mov $0x7FFFFFFFFFFFFFFF, %rax
    mov $0x123456789ABCDEF0, %rdx
    cqto
    mov %rax, %r12
    mov %rdx, %r13
    /* rax is unchanged. */
    LKMC_ASSERT_EQ(%r12, $0x7FFFFFFFFFFFFFFF)
    /* rdx is filled with zeros. */
    LKMC_ASSERT_EQ(%r13, $0)

    /* Quad to Octo: top bit is one: extend with ones. */
    mov $0x8000000000000000, %rax
    mov $0x123456789ABCDEF0, %rdx
    cqto
    mov %rax, %r12
    mov %rdx, %r13
    LKMC_ASSERT_EQ(%r12, $0x8000000000000000)
    LKMC_ASSERT_EQ(%r13, $0xFFFFFFFFFFFFFFFF)

    /* Intel equivalent syntax also accepte by GNU GAS. */
    mov $0x7FFFFFFFFFFFFFFF, %rax
    mov $0x123456789ABCDEF0, %rdx
    cqo
    mov %rax, %r12
    mov %rdx, %r13
    LKMC_ASSERT_EQ(%r12, $0x7FFFFFFFFFFFFFFF)
    LKMC_ASSERT_EQ(%r13, $0)

    /* Smaller size example: Double to Quad.
     * Also zeroes top 32-bits of RDX like many 32 to 64 operaions. */
    mov $0xFFFFFFFF7FFFFFFF, %rax
    mov $0x123456789ABCDEF0, %rdx
    cltd
    mov %rax, %r12
    mov %rdx, %r13
    LKMC_ASSERT_EQ(%r12, $0xFFFFFFFF7FFFFFFF)
    LKMC_ASSERT_EQ(%r13, $0)

    /* Even smaller size example: Word to Doubleword.
     * Unlike the 32-bit one, does not zero out the top 32-bits of RDX. */
    mov $0xFFFFFFFFFFFF7FFF, %rax
    mov $0x123456789ABCDEF0, %rdx
    cwtd
    mov %rax, %r12
    mov %rdx, %r13
    LKMC_ASSERT_EQ(%r12, $0xFFFFFFFFFFFF7FFF)
    LKMC_ASSERT_EQ(%r13, $0x123456789ABC0000)
LKMC_EPILOGUE
