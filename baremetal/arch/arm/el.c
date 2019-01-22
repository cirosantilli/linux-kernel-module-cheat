/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-exception-level */

#include <stdio.h>
#include <inttypes.h>

int main(void) {
    uint32_t cpsr;
    __asm__ ("mrs %0, CPSR" : "=r" (cpsr) : :);
    printf("%" PRIu32 "\n", cpsr & 0x1F);
    return 0;
}
