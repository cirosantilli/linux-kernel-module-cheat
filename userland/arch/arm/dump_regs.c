/* Dump ARM registers that can be read in EL0 (and higher). */

#include <stdio.h>
#include <inttypes.h>

int main(void) {
    uint32_t cpsr;
    uint32_t cpsr_m;
    __asm__ ("mrs %0, cpsr" : "=r" (cpsr) : :);
    printf("CPSR 0x%" PRIX32 "\n", cpsr);
    /* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-exception-levels */
    cpsr_m = cpsr & 0xF;
    printf("CPSR.M 0x%" PRIX32 "\n", cpsr_m);

    return 0;
}
