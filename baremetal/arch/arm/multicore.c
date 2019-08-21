/* https://cirosantilli.com/linux-kernel-module-cheat#arm-multicore */

#include <lkmc.h>

uint64_t spinlock = 0;

__asm__(
".syntax unified\n"
".text\n"
".global lkmc_cpu_not_0\n"
"lkmc_cpu_not_0:\n"
"    cmp r0, 1\n"
"    bne .Lsleep_forever\n"
"    ldr sp, =(stack_top - 0x1000)\n"
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
    lkmc_arm_psci_cpu_on(1, (uint32_t)main_cpu1, 0);
#endif
    while (!spinlock) {
        lkmc_arm_aarch64_wfe();
    }
}
