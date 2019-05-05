/* Like inc.c but less good since we do more work ourselves.
 *
 * Just doing this to test out the "m" memory constraint.
 *
 * GCC 8.2.0 -O0 assembles ldr line to:
 *
 * ....
 * ldr r0, [fp, #-12]
 * ....
 *
 * and `-O3` assembles to:
 *
 * ....
 * ldr r0, [sp]
 * ....
 */

#include <assert.h>
#include <inttypes.h>

int main(void) {
    uint32_t my_local_var = 1;
    __asm__ (
        "ldr r0, %[my_local_var];"
        "add r0, r0, #1;"
        "str r0, %[my_local_var];"
        : [my_local_var] "+m" (my_local_var)
        :
        : "r0"
    );
    assert(my_local_var == 2);
}
