/* count to infinity in n threads.
 *
 * https://github.com/cirosantilli/linux-kernel-module-cheat#pthreads
 *
 * Useful if you need to keep several threads around
 * to test something.
 *
 * Usage:
 *
 * ....
 * ./pthread_count.out 3
 * ....
 *
 * Sample output:
 *
 * ....
 * 0 0
 * 1 0
 * 2 0
 * 1 1
 * 2 1
 * 0 1
 * 1 2
 * 0 2
 * 2 2
 * ....
 *
 * Initial motivation: confirm that:
 *
 * ....
 * ./pthread_count.out 4 &
 * cat /proc/$!/status | grep -E '^Threads:'
 * kill $!
 * ....
 *
 * shows the right thread count:
 *
 * ....
 * Threads:        5
 * ....
 *
 * which is 1 main thread + 4 we spawned!
 */

#define _XOPEN_SOURCE 700
#include <assert.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void* main_thread(void *arg) {
    unsigned long i = 0;
    unsigned int thread_id;
    thread_id = *((unsigned int*)arg);
    while (1) {
        printf("%u %lu\n", thread_id, i);
        i++;
        sleep(1);
    }
    return NULL;
}

int main(int argc, char **argv) {
    pthread_t *threads;
    unsigned int nthreads, i, *thread_args;
    if (argc > 1) {
        nthreads = strtoll(argv[1], NULL, 0);
    } else {
        nthreads = 1;
    }
    threads = malloc(nthreads * sizeof(*threads));
    thread_args = malloc(nthreads * sizeof(*thread_args));
    for (i = 0; i < nthreads; ++i) {
        thread_args[i] = i;
        assert(pthread_create(
            &threads[i],
            NULL,
            main_thread,
            (void*)&thread_args[i]
        ) == 0);
    }
    for (i = 0; i < nthreads; ++i) {
        pthread_join(threads[i], NULL);
    }
    free(thread_args);
    free(threads);
    return EXIT_SUCCESS;
}
