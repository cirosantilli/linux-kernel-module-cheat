/* https://cirosantilli.com/linux-kernel-module-cheat#x86-bit-and-byte-instructions */

#include <lkmc.h>

LKMC_PROLOGUE
    /* 0101 1010 */
    mov $0x5A, %r12
    btc $0, %r12
    LKMC_ASSERT(jnc)
    LKMC_ASSERT_EQ(%r12, $0x5B)

    /* 0101 1010 */
    btc $0, %r12
    LKMC_ASSERT(jc)
    LKMC_ASSERT_EQ(%r12, $0x5A)
LKMC_EPILOGUE
