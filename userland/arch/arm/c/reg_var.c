/* https://github.com/cirosantilli/arm-assembly-cheat#register-variables */

#include <assert.h>
#include <inttypes.h>

int main(void) {
    register uint32_t r0 __asm__ ("r0");
    register uint32_t r1 __asm__ ("r1");
    uint32_t new_r0;
    uint32_t new_r1;
    {
        /* We must set the registers immediately before calling,
         * without making any function calls in between.
         */
        r0 = 1;
        r1 = 2;
        __asm__ (
            /* We intentionally use an explicit r0 and r1 here,
            * just to illustrate that we are certain that the
            * r0 variable will go in r0. Real code would never do this.
            */
            "add %[r0], r0, #1;"
            "add %[r1], r1, #1;"
            /* We have to specify r0 in the constraints.*/
            : [r0] "+r" (r0),
              [r1] "+r" (r1)
            :
            :
        );
        /* When we are done, we must immediatly assign
         * the register variables to regular variables.
         */
        new_r0 = r0;
        new_r1 = r1;
    }
    assert(new_r0 == 2);
    assert(new_r1 == 3);
}
