/* https://cirosantilli.com/linux-kernel-module-cheat#arm-sve-vaddl-instruction */

#include <inttypes.h>
#include <stdio.h>

int main(void) {
    uint64_t vl = 0;
    __asm__ (
        "addvl %[vl], %[vl], #8"
        : [vl] "+r" (vl)
        :
        :
    );
    printf("0x%" PRIX64 "\n", vl);
}
