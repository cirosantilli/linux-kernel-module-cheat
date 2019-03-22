/* https://github.com/cirosantilli/arm-assembly-cheat#calling-convention */

#include <assert.h>
#include <inttypes.h>

uint64_t my_asm_func(void);
/* { return 42; } */
__asm__(
    ".global my_asm_func;"
    "my_asm_func:"
    "mov x0, 42;"
    "ret;"
);

/* Now a more complex example that also calls a C function.
 * We have to store the return value x30 for later because bl modifies it.
 * https://stackoverflow.com/questions/27941220/push-lr-and-pop-lr-in-arm-arch64/34504752#34504752
 * We are not modifying any other callee saved register in this function,
 * since my_c_func is not either (unless GCC has a bug ;-)), so everything else if fine.
 */
uint64_t my_asm_func_2(void);
/* { return my_c_func(); } */
__asm__(
    ".global my_asm_func_2;"
    "my_asm_func_2:"
    "str x30, [sp, -16]!;"
    "bl my_c_func;"
    "ldr x30, [sp], 16;"
    "ret;"
);

uint64_t my_c_func(void) {
    return 42;
}

int main(void) {
    assert(my_asm_func() == 42);
    assert(my_asm_func_2() == 42);
}
