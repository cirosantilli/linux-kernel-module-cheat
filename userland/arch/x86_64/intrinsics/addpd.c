/* https://cirosantilli.com/linux-kernel-module-cheat#gcc-intrinsics */

#include <assert.h>
#include <string.h>

#include <x86intrin.h>

float global_input0[] __attribute__((aligned(16))) = {1.5f, 2.5f, 3.5f, 4.5f};
float global_input1[] __attribute__((aligned(16))) = {5.5f, 6.5f, 7.5f, 8.5f};
float global_output[4] __attribute__((aligned(16)));
float global_expected[] __attribute__((aligned(16))) = {7.0f, 9.0f, 11.0f, 13.0f};

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
        assert(f == 7.0f);
        _MM_EXTRACT_FLOAT(f, output, 2);
        assert(f == 9.0f);
        _MM_EXTRACT_FLOAT(f, output, 1);
        assert(f == 11.0f);
        _MM_EXTRACT_FLOAT(f, output, 0);
        assert(f == 13.0f);

        /* And we also have _mm_cvtss_f32 + _mm_shuffle_ps, */
        assert(_mm_cvtss_f32(output) == 13.0f);
        assert(_mm_cvtss_f32(_mm_shuffle_ps(output, output, 1)) == 11.0f);
        assert(_mm_cvtss_f32(_mm_shuffle_ps(output, output, 2)) ==  9.0f);
        assert(_mm_cvtss_f32(_mm_shuffle_ps(output, output, 3)) ==  7.0f);
    }

    /* Now from memory. */
    {
        __m128 *input0 = (__m128 *)global_input0;
        __m128 *input1 = (__m128 *)global_input1;
        _mm_store_ps(global_output, _mm_add_ps(*input0, *input1));
        assert(!memcmp(global_output, global_expected, sizeof(global_output)));
    }

    /* 64-bit add (addpd). */
    {
        __m128d input0 = _mm_set_pd(1.5, 2.5);
        __m128d input1 = _mm_set_pd(5.5, 6.5);
        __m128d output = _mm_add_pd(input0, input1);
        /* OK, and this is how we get the doubles out:
         * with _mm_cvtsd_f64 + _mm_unpackhi_pd
         * https://stackoverflow.com/questions/19359372/mm-cvtsd-f64-analogon-for-higher-order-floating-point
         */
        assert(_mm_cvtsd_f64(output) == 9.0);
        assert(_mm_cvtsd_f64(_mm_unpackhi_pd(output, output)) == 7.0);
    }

    return 0;
}
