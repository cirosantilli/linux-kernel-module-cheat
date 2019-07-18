/* Dump registers that cannot be read from EL0. */

#include <stdio.h>
#include <inttypes.h>

int main(void) {
    uint32_t sctlr_el1;
    __asm__ ("mrs %0, sctlr_el1" : "=r" (sctlr_el1) : :);
    printf("SCTLR_EL1 0x%" PRIX32 "\n", sctlr_el1);
    printf("SCTLR_EL1.A 0x%" PRIX32 "\n", (sctlr_el1 >> 1) & 1);
    /* https://cirosantilli.com/linux-kernel-module-cheat#arm-paging */
    printf("SCTLR_EL1.M 0x%" PRIX32 "\n", (sctlr_el1 >> 0) & 1);

    uint64_t CurrentEL;
    __asm__ ("mrs %0, CurrentEL;" : "=r" (CurrentEL) : :);
    printf("CurrentEL 0x%" PRIX64 "\n", CurrentEL);
    /* https://cirosantilli.com/linux-kernel-module-cheat#arm-exception-levels */
    printf("CurrentEL.EL 0x%" PRIX64 "\n", CurrentEL >> 2);

    /* https://cirosantilli.com/linux-kernel-module-cheat#arm-paging */
    {
        uint64_t tcr_el1;
        __asm__ ("mrs %0, tcr_el1;" : "=r" (tcr_el1) : :);
        printf("TCR_EL1 0x%" PRIX64 "\n", tcr_el1);

        uint64_t ttbr0_el1;
        __asm__ ("mrs %0, ttbr0_el1;" : "=r" (ttbr0_el1) : :);
        printf("TTBR0_EL1 0x%" PRIX64 "\n", ttbr0_el1);
    }

    return 0;
}
