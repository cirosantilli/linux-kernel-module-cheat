/* https://cirosantilli.com/linux-kernel-module-cheat#dump-regs */

#include <stdio.h>
#include <inttypes.h>

int main(void) {
    uint32_t spsr;
    __asm__ ("mrs %0, spsr" : "=r" (spsr) : :);
    printf("SPSR 0x%" PRIX32 "\n", spsr);
    /* https://cirosantilli.com/linux-kernel-module-cheat#arm-exception-levels */
    printf("SPSR.M 0x%" PRIX32 "\n", spsr & 0xF);

    /* CP14 https://cirosantilli.com/linux-kernel-module-cheat#arm-system-register-instructions */

    uint32_t dbgdidr;
    __asm__ ("mrc p14, 0, %0, c0, c0, 0" : "=r" (dbgdidr) : :);
    printf("DBGDIDR 0x%" PRIX32 "\n", dbgdidr);

#if !LKMC_GEM5
    uint32_t dbgdrar_0;
    uint32_t dbgdrar_1;
    __asm__ ("mrrc p14, 0, %0, %1, c1" : "=r" (dbgdrar_0), "=r" (dbgdrar_1) : :);
    printf("DBGDRAR 0x%" PRIX64 "\n", dbgdrar_0 | ((uint64_t)dbgdrar_1 << 32));
#endif

    /* CP15 https://cirosantilli.com/linux-kernel-module-cheat#arm-system-register-instructions */

    uint32_t midr;
    __asm__ ("mrc p15, 0, %0, c0, c0, 0" : "=r" (midr) : :);
    printf("MIDR 0x%" PRIX32 "\n", midr);
    printf("MIDR.Architecture 0x%" PRIX32 "\n", (midr >> 16) & 0xF);

    uint32_t ctr;
    __asm__ ("mrc p15, 0, %0, c0, c0, 0" : "=r" (ctr) : :);
    printf("CTR 0x%" PRIX32 "\n", ctr);

    uint32_t ttbr0_0;
    uint32_t ttbr0_1;
    __asm__ ("mrrc p15, 0, %0, %1, c2" : "=r" (ttbr0_0), "=r" (ttbr0_1) : :);
    printf("TTBR0 0x%" PRIX64 "\n", ttbr0_0 | ((uint64_t)ttbr0_1 << 32));

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
