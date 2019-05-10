/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-simd
 *
 * Add 4 32-bit integeres in one go.
 */

#include "common.h"

ENTRY
.data
    u32_0:      .long 0xF111F111, 0xF222F222, 0xF333F333, 0xF444F444
    u32_1:      .long 0x15551555, 0x16661666, 0x17771777, 0x18881888
    u32_expect: .long 0x06670666, 0x08890889, 0x0AAB0AAA, 0x0CCD0CCD
.bss
    u32_result: .skip 16
.text
    movups u32_0, %xmm0
    movups u32_1, %xmm1
    paddq %xmm1, %xmm0
    movups %xmm0, u32_result
    ASSERT_MEMCMP(u32_result, u32_expect, $0x10)
EXIT
