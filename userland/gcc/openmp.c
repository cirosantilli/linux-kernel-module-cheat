/* https://github.com/cirosantilli/linux-kernel-module-cheat#openmp */

#include <stdio.h>
#include <stdlib.h>

#include <omp.h>

int main (void) {
    int nthreads, tid;
#pragma omp parallel private(nthreads, tid)
    {
        tid = omp_get_thread_num();
        printf("Hello World from thread = %d\n", tid);
        if (tid == 0) {
            nthreads = omp_get_num_threads();
            printf("Number of threads = %d\n", nthreads);
        }
    }
    return EXIT_SUCCESS;
}
