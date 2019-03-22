/* https://github.com/cirosantilli/arm-assembly-cheat#register-variables */

#include <assert.h>
#include <inttypes.h>

int main(void) {
    register double d0 __asm__ ("d0");
    register double d1 __asm__ ("d1");
    double new_d0;
    double new_d1;
    {
        d0 = 1.5;
        d1 = 2.5;
        __asm__ (
            "fmov d2, 1.5;"
            "fadd %d[d0], d0, d2;"
            "fadd %d[d1], d1, d2;"
            : [d0] "+w" (d0),
              [d1] "+w" (d1)
            :
            : "d2"
        );
        new_d0 = d0;
        new_d1 = d1;
    }
    assert(new_d0 == 3.0);
    assert(new_d1 == 4.0);
}
