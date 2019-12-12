/* https://cirosantilli.com/linux-kernel-module-cheat#arm-baremetal-multicore
 *
 * Beware: things will blow up if the stack for CPU0 grow too much and
 * reaches that of CPU1. This is why it is so hard to do multithreading
 * without an OS that manages paging.
 */

#include <lkmc.h>

uint64_t spinlock = 0;

__asm__(
".text\n"
".global lkmc_cpu_not_0\n"
"lkmc_cpu_not_0:\n"
/* Put all CPUs except CPU1 to sleep. */
"    cmp x0, 1\n"
"    bne .Lsleep_forever\n"
/* Prepare the stack for CPU1. This is what we need
 * this assembly function for. */
"    ldr x0, =(stack_top - 0x1000)\n"
"    mov sp, x0\n"
"    bl main_cpu1\n"
".Lsleep_forever:\n"
"    wfe\n"
"    b .Lsleep_forever\n"
);

static void main_cpu1(void) {
    spinlock = 1;
    lkmc_arm_aarch64_dmb(sy);
    lkmc_arm_aarch64_sev();
    while (1) {
        lkmc_arm_aarch64_wfe();
    }
}

int main(void) {
#if !LKMC_GEM5
    lkmc_aarch64_psci_cpu_on(1, (uint64_t)main_cpu1, 0);
#endif
    while (!spinlock) {
        lkmc_arm_aarch64_wfe();
    }
}
