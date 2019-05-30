/* This is a worse version of scratch.c with hardcoded scratch.
 * https://github.com/cirosantilli/linux-kernel-module-cheat#gcc-inline-assembly-scratch-registers
 */

#include <assert.h>
#include <inttypes.h>

int main(void) {
    uint64_t in1 = 0xFFFFFFFF;
    uint64_t in2 = 1;
    uint64_t out;
    __asm__ (
        "mov %[in2], %%rax;" /* scratch = in2 */
        "add %[in1], %%rax;" /* scratch += in1 */
        "mov %%rax, %[out];" /* out = scratch */
        : [out] "=r" (out)
        : [in1] "r" (in1),
          [in2] "r" (in2)
        : "rax"
    );
    assert(out == 0x100000000);
}
