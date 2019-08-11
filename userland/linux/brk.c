/* https://cirosantilli.com/linux-kernel-module-cheat#brk */

#define _GNU_SOURCE
#include <assert.h>
#include <unistd.h>

int main(void) {
    void *b = sbrk(0);
    int *p = (int *)b;

    /* Move it 2 ints forward */
    brk(p + 2);

    /* Use the ints. */
    *p = 1;
    *(p + 1) = 2;
    assert(*p == 1);
    assert(*(p + 1) == 2);

    /* Deallocate back. */
    brk(b);

    return 0;
}
