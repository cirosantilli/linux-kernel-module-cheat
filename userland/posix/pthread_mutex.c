/* https://cirosantilli.com/linux-kernel-module-cheat#pthread-mutex */

#define _XOPEN_SOURCE 700
#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

unsigned long long niters;
unsigned long long global = 0;
pthread_mutex_t main_thread_mutex = PTHREAD_MUTEX_INITIALIZER;

void* main_thread(void *arg) {
    (void)arg;
    unsigned long long i;
    for (i = 0; i < niters; ++i) {
        pthread_mutex_lock(&main_thread_mutex);
        global++;
        pthread_mutex_unlock(&main_thread_mutex);
    }
    return NULL;
}

int main(int argc, char **argv) {
    pthread_t *threads;
    unsigned long long i, nthreads;

    /* CLI arguments. */
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
    threads = malloc(sizeof(pthread_t) * nthreads);

    /* Action */
    for (i = 0; i < nthreads; ++i)
        pthread_create(&threads[i], NULL, main_thread, NULL);
    for (i = 0; i < nthreads; ++i)
        pthread_join(threads[i], NULL);
    assert(global == nthreads * niters);

    /* Cleanup. */
    free(threads);
    return EXIT_SUCCESS;
}
