// https://cirosantilli.com/linux-kernel-module-cheat#atomic-c */

#if __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_THREADS__)
#include <assert.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <threads.h>

#if LKMC_USERLAND_ATOMIC_STD_ATOMIC
atomic_uint global = 0;
#else
uint64_t global = 0;
#endif

int my_thread_main(void *thr_data) {
    size_t niters = *(size_t *)thr_data;
    for (size_t i = 0; i < niters; ++i) {
#if LKMC_USERLAND_ATOMIC_X86_64_INC
        __asm__ __volatile__ (
            "incq %0;"
            : "+g" (global),
              "+g" (i)
            :
            :
        );
#elif LKMC_USERLAND_ATOMIC_X86_64_LOCK_INC
        __asm__ __volatile__ (
            "lock incq %0;"
            : "+m" (global),
              "+g" (i)
            :
            :
        );
#elif LKMC_USERLAND_ATOMIC_AARCH64_ADD
        __asm__ __volatile__ (
            "add %0, %0, 1;"
            : "+r" (global),
              "+g" (i)
            :
            :
        );
#elif LKMC_USERLAND_ATOMIC_LDAXR_STLXR
        uint64_t scratch64;
        uint64_t scratch32;
        __asm__ __volatile__ (
            "1:"
            "ldaxr %[scratch64], [%[addr]];"
            "add   %[scratch64], %[scratch64], 1;"
            "stlxr %w[scratch32], %[scratch64], [%[addr]];"
            "cbnz  %w[scratch32], 1b;"
            : "=m" (global),
              "+g" (i),
              [scratch64] "=&r" (scratch64),
              [scratch32] "=&r" (scratch32)
            : [addr] "r" (&global)
            :
        );
#elif LKMC_USERLAND_ATOMIC_AARCH64_LDADD
        __asm__ __volatile__ (
            "ldadd %[inc], xzr, [%[addr]];"
            : "=m" (global),
              "+g" (i)
            : [inc] "r" (1),
              [addr] "r" (&global)
            :
        );
#else
        __asm__ __volatile__ (
            ""
            : "+g" (i)
            : "g" (global)
            :
        );
        global++;
#endif
    }
    return 0;
}
#endif

int main(int argc, char **argv) {
#if __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_THREADS__)
    size_t niters, nthreads;
    thrd_t *threads;
    if (argc > 1) {
        nthreads = strtoull(argv[1], NULL, 0);
    } else {
        nthreads = 2;
    }
    if (argc > 2) {
        niters = strtoull(argv[2], NULL, 0);
    } else {
        niters = 10;
    }
    threads = malloc(sizeof(thrd_t) * nthreads);
    for(size_t i = 0; i < nthreads; ++i)
        assert(thrd_create(threads + i, my_thread_main, &niters) == thrd_success);
    for(size_t i = 0; i < nthreads; ++i)
        assert(thrd_join(threads[i], NULL) == thrd_success);
    free(threads);
    uint64_t expect = nthreads * niters;
#if LKMC_USERLAND_ATOMIC_FAIL || \
    LKMC_USERLAND_ATOMIC_X86_64_INC || \
    LKMC_USERLAND_ATOMIC_AARCH64_ADD
    printf("expect %ju\n", (uintmax_t)expect);
    printf("global %ju\n", (uintmax_t)global);
#else
    assert(global == expect);
#endif
#else
    (void)argc;
    (void)argv;
#endif
}

