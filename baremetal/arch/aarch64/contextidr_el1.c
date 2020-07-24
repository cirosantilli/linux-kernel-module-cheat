/* https://cirosantilli.com/linux-kernel-module-cheat#config-pid-in-contextidr */

#include <inttypes.h>

int main(void) {
    for (int i = 0; i < 10; i++) {
        __asm__ ("msr contextidr_el1, %0" : : "r" (i) :);
    }
    return 0;
}
