/* https://github.com/cirosantilli/linux-kernel-module-cheat#inline-assembly */

#include <assert.h>
#include <inttypes.h>

int main(void) {
    uint64_t in1 = 0xFFFFFFFF;
    uint64_t in2 = 0x1;
    uint64_t out;
    __asm__ (
        "lea (%[in1], %[in2]), %[out];"
        : [out] "=r" (out)
        : [in1] "r" (in1),
          [in2] "r" (in2)
        :
    );
    assert(out == 0x100000000);
}
