/* https://cirosantilli.com/linux-kernel-module-cheat#getcpu */

#define _GNU_SOURCE
#include <assert.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

pthread_barrier_t barrier;

void* main_thread(void *arg) {
    (void)arg;
    printf("%d\n", sched_getcpu());
    pthread_barrier_wait(&barrier);
    return NULL;
}

int main(int argc, char **argv) {
    pthread_t *threads;
    unsigned int nthreads, i;
    if (argc > 1) {
        nthreads = strtoll(argv[1], NULL, 0);
    } else {
        nthreads = 1;
    }
    assert(!pthread_barrier_init(&barrier, NULL, nthreads));
    threads = malloc(nthreads * sizeof(*threads));
    for (i = 0; i < nthreads; ++i) {
        assert(pthread_create(
            &threads[i],
            NULL,
            main_thread,
            NULL
        ) == 0);
    }
    for (i = 0; i < nthreads; ++i) {
        pthread_join(threads[i], NULL);
    }
    free(threads);
    assert(!pthread_barrier_destroy(&barrier));
    return EXIT_SUCCESS;
}
