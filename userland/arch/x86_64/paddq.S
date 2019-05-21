/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-paddq-instruction
 *
 * Add a bunch of integers in one go.
 *
 * The different variants basically determine if carries get forwarded or not.
 */

#include "common.h"

LKMC_ENTRY
.data
    input0:       .long 0xF1F1F1F1, 0xF2F2F2F2, 0xF3F3F3F3, 0xF4F4F4F4
    input1:       .long 0x12121212, 0x13131313, 0x14141414, 0x15151515
    paddb_expect: .long 0x03030303, 0x05050505, 0x07070707, 0x09090909
    paddw_expect: .long 0x04030403, 0x06050605, 0x08070807, 0x0A090A09
    paddd_expect: .long 0x04040403, 0x06060605, 0x08080807, 0x0A0A0A09
    paddq_expect: .long 0x04040403, 0x06060606, 0x08080807, 0x0A0A0A0A
.bss
    output:       .skip 16
.text
    movups input1, %xmm1
#define TEST(size) \
    movups input0, %xmm0; \
    padd ## size %xmm1, %xmm0; \
    movups %xmm0, output; \
    LKMC_ASSERT_MEMCMP(output, padd ## size ## _expect, $0x10)

    /* 16x 8-bit */
    TEST(b)
    /* 8x 4-bit */
    TEST(w)
    /* 4x 8-bit */
    /*  4x long */
    TEST(d)
    /* 2x 16-bit */
    TEST(q)
#undef TEST
LKMC_EXIT
