/* https://cirosantilli.com/linux-kernel-module-cheat#userland-physical-address-experiments */

#define _XOPEN_SOURCE 700
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

enum { I0 = 0x12345678 };

static volatile uint32_t i = I0;

int main(void) {
    printf("vaddr %p\n", (void *)&i);
    printf("pid %ju\n", (uintmax_t)getpid());
    while (i == I0) {
        sleep(1);
    }
    printf("i %jx\n", (uintmax_t)i);
    return EXIT_SUCCESS;
}
