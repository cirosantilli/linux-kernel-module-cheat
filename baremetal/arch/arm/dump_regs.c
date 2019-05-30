/* Dump registers that cannot be read from EL0. */

#include <stdio.h>
#include <inttypes.h>

int main(void) {
    uint32_t spsr;
    __asm__ ("mrs %0, spsr" : "=r" (spsr) : :);
    printf("SPSR 0x%" PRIX32 "\n", spsr);
    /* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-exception-levels */
    printf("SPSR.M 0x%" PRIX32 "\n", spsr & 0xF);

#if 0
    /* TODO blows up exception in EL, but works with -machine secure=on. */
    uint32_t nsacr;
    __asm__ ("mrc p15, 0, %0, c1, c1, 2" : "=r" (nsacr) : :);
    printf("NSACR 0x%" PRIX32 "\n", nsacr);
#endif

#if 0
    /* TODO blows up exception. */
    uint32_t mvfr1;
    __asm__ ("vmrs %0, mvfr1" : "=r" (mvfr1) : :);
    printf("MVFR1 0x%" PRIX32 "\n", mvfr1);
#endif

    return 0;
}
