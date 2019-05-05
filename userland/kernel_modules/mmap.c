/* https://github.com/cirosantilli/linux-kernel-module-cheat#mmap */

#define _XOPEN_SOURCE 700
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> /* uintmax_t */
#include <string.h>
#include <sys/mman.h>
#include <unistd.h> /* sysconf */

#include <lkmc/pagemap.h> /* lkmc_pagemap_virt_to_phys_user */

enum { BUFFER_SIZE = 4 };

int main(int argc, char **argv) {
    int fd;
    long page_size;
    char *address1, *address2;
    char buf[BUFFER_SIZE];
    uintptr_t paddr;

    if (argc < 2) {
        printf("Usage: %s <mmap_file>\n", argv[0]);
        return EXIT_FAILURE;
    }
    page_size = sysconf(_SC_PAGE_SIZE);
    printf("open pathname = %s\n", argv[1]);
    fd = open(argv[1], O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("open");
        assert(0);
    }
    printf("fd = %d\n", fd);

    /* mmap twice for double fun. */
    puts("mmap 1");
    address1 = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (address1 == MAP_FAILED) {
        perror("mmap");
        assert(0);
    }
    puts("mmap 2");
    address2 = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (address2 == MAP_FAILED) {
        perror("mmap");
        return EXIT_FAILURE;
    }
    assert(address1 != address2);

    /* Read and modify memory. */
    puts("access 1");
    assert(!strcmp(address1, "asdf"));
    /* vm_fault */
    puts("access 2");
    assert(!strcmp(address2, "asdf"));
    /* vm_fault */
    strcpy(address1, "qwer");
    /* Also modified. So both virtual addresses point to the same physical address. */
    assert(!strcmp(address2, "qwer"));

    /* Check that the physical addresses are the same.
     * They are, but TODO why virt_to_phys on kernel gives a different value? */
    assert(!lkmc_pagemap_virt_to_phys_user(&paddr, getpid(), (uintptr_t)address1));
    printf("paddr1 = 0x%jx\n", (uintmax_t)paddr);
    assert(!lkmc_pagemap_virt_to_phys_user(&paddr, getpid(), (uintptr_t)address2));
    printf("paddr2 = 0x%jx\n", (uintmax_t)paddr);

    /* Check that modifications made from userland are also visible from the kernel. */
    read(fd, buf, BUFFER_SIZE);
    assert(!memcmp(buf, "qwer", BUFFER_SIZE));

    /* Modify the data from the kernel, and check that the change is visible from userland. */
    write(fd, "zxcv", 4);
    assert(!strcmp(address1, "zxcv"));
    assert(!strcmp(address2, "zxcv"));

    /* Cleanup. */
    puts("munmap 1");
    if (munmap(address1, page_size)) {
        perror("munmap");
        assert(0);
    }
    puts("munmap 2");
    if (munmap(address2, page_size)) {
        perror("munmap");
        assert(0);
    }
    puts("close");
    close(fd);
    return EXIT_SUCCESS;
}
