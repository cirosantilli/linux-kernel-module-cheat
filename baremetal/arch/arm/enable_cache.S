/* TODO not working on gem5. Maybe we need to flush some pipelines after the sysreg accesses. */

#include <lkmc.h>

#define NELEM 4
#define ELEM_SIZE 4

.data;
.align 4
my_array_1:
    .word 0x11111111, 0x22222222, 0x33333333, 0x44444444
my_array_2:
    .word 0x99999999, 0xAAAAAAAA, 0xBBBBBBBB, 0xCCCCCCCC

LKMC_PROLOGUE

    /* Modify the array. */
    ldr r4, =my_array_1
    ldr r5, =0x55555555
    ldr r6, =0x66666666
    ldr r7, =0x77777777
    ldr r8, =0x88888888
    stmia r4, {r5-r8}

    mrc p15, 0, r0, c1, c0, 0
    orr r0, r0, (1 << 2)  /* SCTLR.C */
    orr r0, r0, (1 << 28) /* SCTLR.TRE */
    mcr p15, 0, r0, c1, c0, 0

    mrc p15, 0, r0, c9, c2, 1
    orr r0, r0, 1         /* NMRR.IR0 */
    orr r0, r0, (1 << 16) /* NMRR.OR0 */
    mcr p15, 0, r0, c9, c2, 1

    /* Modify the again. */
    ldr r4, =my_array_1
    ldr r5, =0x99999999
    ldr r6, =0xAAAAAAAA
    ldr r7, =0xBBBBBBBB
    ldr r8, =0xCCCCCCCC
    stmia r4, {r5-r8}

    /* Verify that my_array_0 changed and is equal to my_array_1. */
    LKMC_ASSERT_MEMCMP(my_array_1, my_array_2, =0x10)

LKMC_EPILOGUE
