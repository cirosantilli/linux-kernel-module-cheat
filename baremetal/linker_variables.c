/* https://cirosantilli.com/linux-kernel-module-cheat#baremetal-linker-script */

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

extern int32_t lkmc_argc;
extern int32_t lkmc_heap_low;

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    printf("&lkmc_heap_low %p\n", (void *)&lkmc_heap_low);
    printf("&lkmc_argc %p\n", (void *)&lkmc_argc);
    printf("argc %d\n", argc);
    printf("argv %p\n", (void *)argv);
    printf("lkmc_argc %" PRId32 "\n", lkmc_argc);
}
