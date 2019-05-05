/* https://github.com/cirosantilli/linux-kernel-module-cheat#inline-assembly */

#include <assert.h>
#include <inttypes.h>

int main(void) {
    uint64_t io = 1;
    __asm__ (
        "lea 1(%[io]), %[io];"
        : [io] "+r" (io)
        :
        :
    );
    assert(io == 2);
}
