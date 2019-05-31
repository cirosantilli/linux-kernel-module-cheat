/* https://github.com/cirosantilli/linux-kernel-module-cheat#gcc-intrinsics */

#include <assert.h>

#include <x86intrin.h>

int main(void) {

    /* 32-bit add hello world. */
    {
        __m128i input0 = _mm_set_epi32(1, 2, 3, 4);
        __m128i input1 = _mm_set_epi32(5, 6, 7, 8);
        __m128i output = _mm_add_epi32(input0, input1);
        assert(_mm_extract_epi32(output, 3) == 6);
        assert(_mm_extract_epi32(output, 2) == 8);
        assert(_mm_extract_epi32(output, 1) == 10);
        assert(_mm_extract_epi32(output, 0) == 12);
    }

    /* Now a bunch of other sizes. */
    {
        __m128i input0 = _mm_set_epi32(0xF1F1F1F1, 0xF2F2F2F2, 0xF3F3F3F3, 0xF4F4F4F4);
        __m128i input1 = _mm_set_epi32(0x12121212, 0x13131313, 0x14141414, 0x15151515);
        __m128i output;

        /* 8-bit integers (paddb) */
        output = _mm_add_epi8(input0, input1);
        assert(_mm_extract_epi32(output, 3) == 0x03030303);
        assert(_mm_extract_epi32(output, 2) == 0x05050505);
        assert(_mm_extract_epi32(output, 1) == 0x07070707);
        assert(_mm_extract_epi32(output, 0) == 0x09090909);

        /* 32-bit integers (paddw) */
        output = _mm_add_epi16(input0, input1);
        assert(_mm_extract_epi32(output, 3) == 0x04030403);
        assert(_mm_extract_epi32(output, 2) == 0x06050605);
        assert(_mm_extract_epi32(output, 1) == 0x08070807);
        assert(_mm_extract_epi32(output, 0) == 0x0A090A09);

        /* 32-bit integers (paddd) */
        output = _mm_add_epi32(input0, input1);
        assert(_mm_extract_epi32(output, 3) == 0x04040403);
        assert(_mm_extract_epi32(output, 2) == 0x06060605);
        assert(_mm_extract_epi32(output, 1) == 0x08080807);
        assert(_mm_extract_epi32(output, 0) == 0x0A0A0A09);

        /* 64-bit integers (paddq) */
        output = _mm_add_epi64(input0, input1);
        assert(_mm_extract_epi32(output, 3) == 0x04040404);
        assert(_mm_extract_epi32(output, 2) == 0x06060605);
        assert(_mm_extract_epi32(output, 1) == 0x08080808);
        assert(_mm_extract_epi32(output, 0) == 0x0A0A0A09);
    }

    return 0;
}
