/* https://cirosantilli.com/linux-kernel-module-cheat#getcpu */

#define _GNU_SOURCE
#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>

void* main_thread(void *arg) {
    (void)arg;
    unsigned cpu = 1, numa;
    int err, ret;
    assert(!syscall(SYS_getcpu, &cpu, &numa, NULL));
    err = errno;
    if (ret == -1) {
        printf("%d\n", err);
        perror("getcpu");
    }
    printf("%d %u %u\n", ret, cpu, numa);
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
    return EXIT_SUCCESS;
}
