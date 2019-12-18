/* https://cirosantilli.com/linux-kernel-module-cheat#gcc-inline-assembly */

#include <assert.h>
#include <inttypes.h>

int main(void) {
    uint32_t io = 1;
    __asm__ (
        "add %w[io], %w[io], 1;"
        : [io] "+r" (io)
        :
        :
    );
    assert(io == 2);
}
