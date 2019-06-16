/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-multicore */

#include <lkmc.h>

LKMC_PROLOGUE
    mov r0, 0
    ldr r1, =spinlock
    str r0, [r1]
    /* Get CPU ID. */
    mrc p15, 0, r1, c0, c0, 5
    ands r1, r1, 3
    beq cpu0_only
.Lcpu1_only:
    mov r0, 1
    ldr r1, =spinlock
    str r0, [r1]
    dmb sy
    sev
.Lcpu1_sleep_forever:
    wfe
    b .Lcpu1_sleep_forever
cpu0_only:
#if !LKMC_GEM5
    /* PSCI CPU_ON. */
    ldr r0, =0x84000003
    mov r1, 1
    ldr r2, =.Lcpu1_only
    mov r3, 0
    hvc 0
#endif
spinlock_start:
    ldr r0, spinlock
    wfe
    cmp r0, 0
    beq spinlock_start
LKMC_EPILOGUE
spinlock:
    .skip 4
