/* Dump as many registers as we feel like to see initial CPU state. */

#include <stdio.h>
#include <inttypes.h>

int main(void) {
    uint32_t sctlr_el1;
    __asm__ ("mrs %0, sctlr_el1" : "=r" (sctlr_el1) : :);
    printf("sctlr_el1 0x%" PRIx32 "\n", sctlr_el1);
    return 0;
}
