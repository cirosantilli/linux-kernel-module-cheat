/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-paddq-instruction
 *
 * Add several integers in one go.
 *
 * The different variants basically determine integer size, which basically
 * determines if carries get forwarded or not.
 */

#include <lkmc.h>

LKMC_PROLOGUE
.data
    .align 16
    input0:       .long 0xF1F1F1F1, 0xF2F2F2F2, 0xF3F3F3F3, 0xF4F4F4F4
    input1:       .long 0x12121212, 0x13131313, 0x14141414, 0x15151515
    paddb_expect: .long 0x03030303, 0x05050505, 0x07070707, 0x09090909
    paddw_expect: .long 0x04030403, 0x06050605, 0x08070807, 0x0A090A09
    paddd_expect: .long 0x04040403, 0x06060605, 0x08080807, 0x0A0A0A09
    paddq_expect: .long 0x04040403, 0x06060606, 0x08080807, 0x0A0A0A0A
.bss
    .align 16
    output:       .skip 16
.text
    movaps input1, %xmm1

    /* 16x 8bit */
    movaps input0, %xmm0
    paddb %xmm1, %xmm0
    movaps %xmm0, output
    LKMC_ASSERT_MEMCMP(output, paddb_expect, $0x10)

    /* 8x 16-bit */
    movaps input0, %xmm0
    paddw %xmm1, %xmm0
    movaps %xmm0, output
    LKMC_ASSERT_MEMCMP(output, paddw_expect, $0x10)

    /* 4x 32-bit */
    movaps input0, %xmm0
    paddd %xmm1, %xmm0
    movaps %xmm0, output
    LKMC_ASSERT_MEMCMP(output, paddd_expect, $0x10)

    /* 2x 64-bit */
    movaps input0, %xmm0
    paddq %xmm1, %xmm0
    movaps %xmm0, output
    LKMC_ASSERT_MEMCMP(output, paddq_expect, $0x10)
LKMC_EPILOGUE
