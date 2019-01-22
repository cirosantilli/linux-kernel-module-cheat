/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-exception-level */

#include <stdio.h>
#include <inttypes.h>

int main(void) {
    register uint32_t r0 __asm__ ("r0");
    __asm__ ("mrs r0, CPSR" : : : "%r0");
    printf("%" PRIu32 "\n", r0 & 0x1F);
    return 0;
}
