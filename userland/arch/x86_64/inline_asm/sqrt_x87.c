/* https://github.com/cirosantilli/linux-kernel-module-cheat#gcc-inline-assembly */

#include <assert.h>

int main(void) {
    double io = 4.0;
    __asm__ (
        "fsqrt"
        : "+t" (io)
        :
        :
    );
    assert(io == 2.0);
    return 0;
}
