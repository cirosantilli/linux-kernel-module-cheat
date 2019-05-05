/* https://github.com/cirosantilli/linux-kernel-module-cheat#userland-physical-address-experiments */

#define _XOPEN_SOURCE 700
#include <stdio.h> /* printf */
#include <stdlib.h> /* EXIT_SUCCESS, EXIT_FAILURE, strtoull */

#include <lkmc/pagemap.h> /* lkmc_pagemap_virt_to_phys_user */

int main(int argc, char **argv) {
    pid_t pid;
    uintptr_t vaddr, paddr = 0;

    if (argc < 3) {
        printf("Usage: %s pid vaddr\n", argv[0]);
        return EXIT_FAILURE;
    }
    pid = strtoull(argv[1], NULL, 0);
    vaddr = strtoull(argv[2], NULL, 0);
    if (lkmc_pagemap_virt_to_phys_user(&paddr, pid, vaddr)) {
        fprintf(stderr, "error: lkmc_pagemap_virt_to_phys_user\n");
        return EXIT_FAILURE;
    };
    printf("0x%jx\n", (uintmax_t)paddr);
    return EXIT_SUCCESS;
}
