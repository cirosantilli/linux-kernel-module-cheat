/* https://github.com/cirosantilli/linux-kernel-module-cheat#gcc-intrinsics */

#include <assert.h>
#include <stdio.h>

#include <x86intrin.h>

int main(void) {
    __m128i input0 = _mm_set_epi32(1, 2, 3, 4);
    __m128i input1 = _mm_set_epi32(5, 6, 7, 8);
    __m128i output = _mm_add_epi32(input0, input1);
    printf("%d\n", (int)output[3]);
    assert(output[0] == 6);
    assert(output[1] == 8);
    assert(output[2] == 10);
    assert(output[3] == 12);
    return 0;
}
