/* Spawn N threads that print their TID with pthread_self and other
 * ID-like information for multiple threads.
 *
 * https://github.com/cirosantilli/linux-kernel-module-cheat#pthreads
 *
 * Sample usage:
 *
 * ....
 * ./pthread_tid.out 4
 * ....
 *
 * Sample output:
 *
 * ....
 * 0 tid: 139852943714048
 * tid, getpid(), pthread_self() = 0, 13709, 139852943714048
 * tid, getpid(), pthread_self() = 1, 13709, 139852935321344
 * 1 tid: 139852935321344
 * 2 tid: 139852926928640
 * tid, getpid(), pthread_self() = 2, 13709, 139852926928640
 * 3 tid: 139852918535936
 * tid, getpid(), pthread_self() = 3, 13709, 139852918535936
 * ....
 *
 * Note how the PID is the same for all threads.
 */

#define _XOPEN_SOURCE 700
#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void* main_thread(void *arg) {
    int argument;
    argument = *((int*)arg);
    printf(
        "tid, getpid(), pthread_self() = "
        "%d, %ju, %ju\n",
        argument,
        (uintmax_t)getpid(),
        (uintmax_t)pthread_self()
    );
    return NULL;
}

int main(int argc, char**argv) {
    pthread_t *threads;
    unsigned int nthreads, i, *thread_args;
    int rc;

    /* CLI arguments. */
    if (argc > 1) {
        nthreads = strtoll(argv[1], NULL, 0);
    } else {
        nthreads = 1;
    }
    threads = malloc(nthreads * sizeof(*threads));
    thread_args = malloc(nthreads * sizeof(*thread_args));

    /* main thread for comparison. */
    printf(
        "tid, getpid(), pthread_self() = "
        "main, %ju, %ju\n",
        (uintmax_t)getpid(),
        (uintmax_t)pthread_self()
    );

    /* Create all threads */
    for (i = 0; i < nthreads; ++i) {
        thread_args[i] = i;
        rc = pthread_create(
            &threads[i],
            NULL,
            main_thread,
            (void*)&thread_args[i]
        );
        if (rc != 0) {
            errno = rc;
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
        assert(rc == 0);
        printf("%d tid: %ju\n", i, (uintmax_t)threads[i]);
    }

    /* Wait for all threads to complete */
    for (i = 0; i < nthreads; ++i) {
        rc = pthread_join(threads[i], NULL);
        if (rc != 0) {
            printf("%s\n", strerror(rc));
            exit(EXIT_FAILURE);
        }
    }

    /* Cleanup. */
    free(thread_args);
    free(threads);
    return EXIT_SUCCESS;
}
