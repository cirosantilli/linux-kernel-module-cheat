/* assert 0x12345678 + 1 == 0x12345679 */

#include <lkmc/m5ops.h>

.global main
main:
    adr r0, myvar
    ldr r1, [r0]
    add r1, r1, #1
    str r1, [r0]
    movw r2, #0x5679
    movt r2, #0x1234
    cmp r1, r2
    beq ok
    LKMC_M5OPS_FAIL_1_ASM
ok:
    LKMC_M5OPS_EXIT_ASM
myvar:
    .word 0x12345678
