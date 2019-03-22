/* https://stackoverflow.com/questions/53960240/armv8-floating-point-output-inline-assembly */

#include <assert.h>

int main(void) {
    float my_float = 1.5;
    __asm__ (
        "vmov s0, 1.0;"
        "vadd.f32 %[my_float], %[my_float], s0;"
        : [my_float] "+t" (my_float)
        :
        : "s0"
    );
    assert(my_float == 2.5);

    /* Undocumented %P
     * https://gcc.gnu.org/bugzilla/show_bug.cgi?id=89482
     */
    double my_double = 1.5;
    __asm__ (
        "vmov.f64 d0, 1.0;"
        "vadd.f64 %P[my_double], %P[my_double], d0;"
        : [my_double] "+w" (my_double)
        :
        : "d0"
    );
    assert(my_double == 2.5);
}
