/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-exception-level */

#include <stdio.h>
#include <inttypes.h>

int main(void) {
    register uint64_t x0 __asm__ ("x0");
    __asm__ ("mrs x0, CurrentEL;" : : : "%x0");
    printf("%" PRIu64 "\n", x0 >> 2);
    return 0;
}
