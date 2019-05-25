/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-exception-levels */

#include <stdio.h>
#include <inttypes.h>

int main(void) {
    uint64_t el;
    __asm__ ("mrs %0, CurrentEL;" : "=r" (el) : :);
    printf("%" PRIu64 "\n", el >> 2);
    return 0;
}
