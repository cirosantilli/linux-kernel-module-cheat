/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-bit-and-byte-instructions */

#include <lkmc.h>

LKMC_PROLOGUE
    /* 0101 1010 */
    mov $0x5A, %r12

    bt $0, %r12w
    LKMC_ASSERT(jnc)

    bt $1, %r12w
    LKMC_ASSERT(jc)

    bt $2, %r12w
    LKMC_ASSERT(jnc)

    bt $3, %r12w
    LKMC_ASSERT(jc)

    bt $4, %r12w
    LKMC_ASSERT(jc)

    bt $5, %r12w
    LKMC_ASSERT(jnc)

    bt $6, %r12w
    LKMC_ASSERT(jc)

    bt $7, %r12w
    LKMC_ASSERT(jnc)

    /* The register is unchanged. */
    LKMC_ASSERT_EQ(%r12, $0x5A)

#if 0
    /* There is no Byte decoding for bt:
     * Error: operand size mismatch for `bt'
     */
    bt $0, %r12b
#endif
LKMC_EPILOGUE
