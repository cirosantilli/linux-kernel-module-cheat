/* https://github.com/cirosantilli/linux-kernel-module-cheat#x86-rdtscp-instruction */

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <x86intrin.h>

int main(void) {
    uint32_t pid;
    printf("0x%016" PRIX64 "\n", (uint64_t)__rdtscp(&pid));
    printf("0x%08" PRIX32 "\n", pid);
    return EXIT_SUCCESS;
}
