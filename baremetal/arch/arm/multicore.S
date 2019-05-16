/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-multicore */

.global main
main:
    mov r0, #0
    ldr r1, =spinlock
    str r0, [r1]
    /* Get CPU ID. */
    mrc p15, 0, r1, c0, c0, 5
    ands r1, r1, #3
    beq cpu0_only
cpu1_only:
    mov r0, #1
    ldr r1, =spinlock
    str r0, [r1]
    dmb sy
    sev
cpu1_sleep_forever:
    wfe
    b cpu1_sleep_forever
cpu0_only:
#if !defined(GEM5)
    /* PSCI CPU_ON. */
    ldr r0, =0x84000003
    mov r1, #1
    ldr r2, =cpu1_only
    mov r3, #0
    hvc 0
#endif
spinlock_start:
    ldr r0, spinlock
    wfe
    cmp r0, #0
    beq spinlock_start
    mov r0, #0
    bx lr
spinlock:
    .skip 4
