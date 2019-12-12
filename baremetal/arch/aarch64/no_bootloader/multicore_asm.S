/* https://cirosantilli.com/linux-kernel-module-cheat#arm-baremetal-multicore
 *
 * This has to be in no_bootloader
 */

#include <lkmc.h>

.global lkmc_start
lkmc_start:
    /* Reset spinlock. */
    mov x0, 0
    ldr x1, =.Lspinlock
    str x0, [x1]

    /* Read cpu id into x1.
     * TODO: cores beyond 4th?
     * Mnemonic: Main Processor ID Register
     */
    mrs x1, mpidr_el1
    ands x1, x1, 3
    beq .Lcpu0_only
.Lcpu1_only:
    /* Only CPU 1 reaches this point and sets the spinlock. */
    mov x0, 1
    ldr x1, =.Lspinlock
    str x0, [x1]
    /* Ensure that CPU 0 sees the write right now.
     * Optional, but could save some useless CPU 1 loops.
     */
    dmb sy
    /* Wake up CPU 0 if it is sleeping on wfe.
     * Optional, but could save power on a real system.
     */
    sev
.Lcpu1_sleep_forever:
    /* Hint CPU 1 to enter low power mode.
     * Optional, but could save power on a real system.
     */
    wfe
    b .Lcpu1_sleep_forever
.Lcpu0_only:
    /* Only CPU 0 reaches this point. */

#if !LKMC_GEM5
    /* Wake up CPU 1 from initial sleep!
     * See:https://cirosantilli.com/linux-kernel-module-cheat#arm-psci
     */
    /* PCSI function identifier: CPU_ON. */
    ldr w0, =0xc4000003
    /* Argument 1: target_cpu */
    mov x1, 1
    /* Argument 2: entry_point_address */
    ldr x2, =.Lcpu1_only
    /* Argument 3: context_id */
    mov x3, 0
    /* Unused hvc args: the Linux kernel zeroes them,
     * but I don't think it is required.
     */
#if 0
    mov x4, 0
    mov x5, 0
    mov x6, 0
    mov x7, 0
#endif
    hvc 0
#endif

.Lspinlock_start:
    ldr x0, .Lspinlock
    /* Hint CPU 0 to enter low power mode. */
    wfe
    cbz x0, .Lspinlock_start

    mov x0, 0
    bl _exit

.Lspinlock:
    .skip 8
