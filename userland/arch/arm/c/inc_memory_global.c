/* GCC 8.2.0 -O0 and -O3 assembles ldr line to:
 *
 * ....
 * movw r3, #<lower address part>
 * movt r3, #<higher address part>
 * ldr r0, [r3]
 * ....
 */

#include <assert.h>
#include <inttypes.h>

uint32_t my_global_var = 1;

int main(void) {
    __asm__ (
        "ldr r0, %[my_global_var];"
        "add r0, r0, #1;"
        "str r0, %[my_global_var];"
        : [my_global_var] "+m" (my_global_var)
        :
        : "r0"
    );
    assert(my_global_var == 2);
}
