/* https://cirosantilli.com/linux-kernel-module-cheat#gcc-inline-assembly-register-variables */

#include <assert.h>
#include <inttypes.h>

int main(void) {
    register uint64_t x0 __asm__ ("x0");
    register uint64_t x1 __asm__ ("x1");
    uint64_t new_x0;
    uint64_t new_x1;
    {
        x0 = 1;
        x1 = 2;
        __asm__ (
            "add %[x0], x0, #1;"
            "add %[x1], x1, #1;"
            : [x0] "+r" (x0),
              [x1] "+r" (x1)
            :
            :
        );
        new_x0 = x0;
        new_x1 = x1;
    }
    assert(new_x0 == 2);
    assert(new_x1 == 3);
}
