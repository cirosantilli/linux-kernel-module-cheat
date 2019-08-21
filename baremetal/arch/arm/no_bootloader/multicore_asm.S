/* https://cirosantilli.com/linux-kernel-module-cheat#arm-multicore */

#include <lkmc.h>

.global lkmc_start
lkmc_start:
    mov r0, 0
    ldr r1, =.Lspinlock
    str r0, [r1]
    /* Get CPU ID. */
    mrc p15, 0, r1, c0, c0, 5
    ands r1, r1, 3
    beq .Lcpu0_only
.Lcpu1_only:
    mov r0, 1
    ldr r1, =.Lspinlock
    str r0, [r1]
    dmb sy
    sev
.Lcpu1_sleep_forever:
    wfe
    b .Lcpu1_sleep_forever
.Lcpu0_only:
#if !LKMC_GEM5
    /* PSCI CPU_ON. */
    ldr r0, =0x84000003
    mov r1, 1
    ldr r2, =.Lcpu1_only
    mov r3, 0
    hvc 0
#endif
.Lspinlock_start:
    ldr r0, .Lspinlock
    wfe
    cmp r0, 0
    beq .Lspinlock_start
    mov r0, 0
    bl _exit
.Lspinlock:
    .skip 4
