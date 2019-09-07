/* https://cirosantilli.com/linux-kernel-module-cheat#c-multithreading */

#if __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_THREADS__)
#include <stdatomic.h>
#include <stdio.h>
#include <threads.h>
#include <string.h>
#include <stdlib.h>

atomic_int acnt;
int cnt;
size_t niters;

int f(void *thr_data) {
    (void)thr_data;
    for (size_t i = 0; i < niters; ++i) {
        ++cnt;
        ++acnt;
    }
    return 0;
}
#endif

int main(int argc, char **argv) {
#if __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_THREADS__)
    size_t nthreads;
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
        thrd_create(threads + i, f, NULL);
    for(size_t i = 0; i < nthreads; ++i)
        thrd_join(threads[i], NULL);
    free(threads);
    printf("atomic %u\n", acnt);
    printf("non-atomic %u\n", cnt);
#else
    (void)argc;
    (void)argv;
#endif
}
