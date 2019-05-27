/* Dump as many registers as we feel like to see initial CPU state. */

#include <stdio.h>
#include <inttypes.h>

int main(void) {
    uint32_t sctlr_el1;
    __asm__ ("mrs %0, sctlr_el1" : "=r" (sctlr_el1) : :);
    printf("SCTLR_EL1 0x%" PRIX32 "\n", sctlr_el1);
    printf("SCTLR_EL1.A 0x%" PRIX32 "\n", (sctlr_el1 >> 1) & 1);

    uint64_t CurrentEL;
    __asm__ ("mrs %0, CurrentEL;" : "=r" (CurrentEL) : :);
    printf("CurrentEL 0x%" PRIX64 "\n", CurrentEL);
    /* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-exception-levels */
    printf("CurrentEL.EL 0x%" PRIX64 "\n", CurrentEL >> 2);
    return 0;
}
