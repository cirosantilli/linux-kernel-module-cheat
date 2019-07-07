/* https://cirosantilli.com/linux-kernel-module-cheat#gcc-inline-assembly-scratch-registers */

#include <assert.h>
#include <inttypes.h>

int main(void) {
    uint64_t in1 = 0xFFFFFFFF;
    uint64_t in2 = 1;
    uint64_t out;
    uint64_t scratch;
    __asm__ (
        "mov %[in2], %[scratch];" /* scratch = in2 */
        "add %[in1], %[scratch];" /* scratch += in1 */
        "mov %[scratch], %[out];" /* out = scratch */
        : [scratch] "=&r" (scratch),
          [out] "=r" (out)
        : [in1] "r" (in1),
          [in2] "r" (in2)
        :
    );
    assert(out == 0x100000000);
}
