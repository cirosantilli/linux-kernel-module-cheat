/* Increment a variable in inline assembly. */

#include <assert.h>
#include <inttypes.h>

int main(void) {
    uint32_t my_local_var = 1;
    __asm__ (
        "add %[my_local_var], %[my_local_var], #1;"
        : [my_local_var] "+r" (my_local_var)
        :
        :
    );
    assert(my_local_var == 2);
}
