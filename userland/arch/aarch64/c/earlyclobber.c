/* https://github.com/cirosantilli/linux-kernel-module-cheat#gcc-inline-assembly-early-clobbers */

#include <assert.h>
#include <inttypes.h>

int main(void) {
    uint64_t in = 1;
    uint64_t out;
    __asm__ (
        "add %[out], %[in], 1;"
        "add %[out], %[in], 1;"
        : [out] "=&r" (out)
        : [in] "r" (in)
        :
    );
    assert(out == 2);
}
