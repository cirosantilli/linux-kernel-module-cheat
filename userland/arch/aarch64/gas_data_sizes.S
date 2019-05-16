/* https://github.com/cirosantilli/linux-kernel-module-cheat#gnu-gas-assembler data sizes */

#include "common.h"

ENTRY
#define ASSERT_DIFF(label1, label2, result) \
    adr x0, label1; \
    adr x1, label2; \
    sub x0, x1, x0; \
    ASSERT_EQ(x0, result)

    ASSERT_DIFF(mybyte, myword, 1)
    ASSERT_DIFF(myword, mylong, 4)
    ASSERT_DIFF(mylong, myquad, 4)
    ASSERT_DIFF(myquad, myocta, 8)
    ASSERT_DIFF(myocta, theend, 16)
#undef ASSERT_DIF
EXIT
mybyte:
    .byte 0x12
myword:
    .word 0x1234
mylong:
    .long 0x12345678
myquad:
    .quad 0x123456789ABCDEF0
myocta:
    .octa 0x123456789ABCDEF0123456789ABCDEF0
theend:
