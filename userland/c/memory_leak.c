/* https://cirosantilli.com/linux-kernel-module-cheat#memory-leaks */

#include <stdlib.h>

void * my_malloc(size_t n) {
    return malloc(n);
}

void leaky(size_t n, int do_leak) {
    void *p = my_malloc(n);
    if (!do_leak) {
        free(p);
    }
}

int main(void) {
    leaky(0x10, 0);
    leaky(0x10, 1);
    leaky(0x100, 0);
    leaky(0x100, 1);
    leaky(0x1000, 0);
    leaky(0x1000, 1);
}
