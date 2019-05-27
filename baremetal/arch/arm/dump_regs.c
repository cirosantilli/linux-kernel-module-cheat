#include <stdio.h>
#include <inttypes.h>

int main(void) {
    uint32_t cpsr;
    __asm__ ("mrs %0, cpsr" : "=r" (cpsr) : :);
    printf("CPSR 0x%" PRIX32 "\n", cpsr);
    /* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-exception-levels */
    printf("CPSR.M 0x%" PRIX32 "\n", cpsr & 0xF);

    /* TODO this is blowing up an exception, how to I read from it? */
    /*uint32_t mvfr1;*/
    /*__asm__ ("vmrs %0, mvfr1" : "=r" (mvfr1) : :);*/
    /*printf("MVFR1 0x%" PRIX32 "\n", mvfr1);*/

    return 0;
}
