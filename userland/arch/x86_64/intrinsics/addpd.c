/* https://github.com/cirosantilli/linux-kernel-module-cheat#gcc-intrinsics */

#include <assert.h>

#include <x86intrin.h>

int main(void) {

    /* 32-bit add (addps). */
    {
        __m128 input0 = _mm_set_ps(1.5f, 2.5f, 3.5f, 4.5f);
        __m128 input1 = _mm_set_ps(5.5f, 6.5f, 7.5f, 8.5f);
        __m128 output = _mm_add_ps(input0, input1);
        /* _mm_extract_ps returns int instead of float:
         * * https://stackoverflow.com/questions/5526658/intel-sse-why-does-mm-extract-ps-return-int-instead-of-float
         * * https://stackoverflow.com/questions/3130169/how-to-convert-a-hex-float-to-a-float-in-c-c-using-mm-extract-ps-sse-gcc-inst
         * so we must use instead: _MM_EXTRACT_FLOAT
         */
        float f;
        _MM_EXTRACT_FLOAT(f, output, 3);
        assert(f ==  7.0f);
        _MM_EXTRACT_FLOAT(f, output, 2);
        assert(f ==  9.0f);
        _MM_EXTRACT_FLOAT(f, output, 1);
        assert(f ==  11.0f);
        _MM_EXTRACT_FLOAT(f, output, 0);
        assert(f ==  13.0f);
    }

    /* 64-bit add (addpd). */
    {
        __m128d input0 = _mm_set_pd(1.5, 2.5);
        __m128d input1 = _mm_set_pd(5.5, 6.5);
        __m128d output = _mm_add_pd(input0, input1);
        double d;
        /* TODO: there is no _MM_EXTRACT_DOUBLE, and the asserts below fail. */
#if 0
        _MM_EXTRACT_FLOAT(d, output, 1);
        assert(d ==  7.0);
        _MM_EXTRACT_FLOAT(d, output, 0);
        assert(d ==  9.0);
#endif
    }

    return 0;
}
