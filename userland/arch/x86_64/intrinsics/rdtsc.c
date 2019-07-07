/* https://cirosantilli.com/linux-kernel-module-cheat#x86-rdtsc-instruction */

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <x86intrin.h>

int main(void) {
    printf("0x%016" PRIX64 "\n", (uint64_t)__rdtsc());
    return EXIT_SUCCESS;
}
