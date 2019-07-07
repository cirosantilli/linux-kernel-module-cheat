/* https://cirosantilli.com/linux-kernel-module-cheat#your-first-binutils-hack */

#include <assert.h>
#include <inttypes.h>

int main(void) {
    uint64_t in = 0xFFFFFFFF;
    uint64_t out = 0;
    __asm__ (
        "mov %[in], %%rax;"
        "inc %%rax;"
        "movq %%rax, %[out]"
        : [out] "=g" (out)
        : [in] "g" (in)
        : "rax"
    );
    assert(out == in + 1);
}
