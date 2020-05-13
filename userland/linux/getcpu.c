/* https://cirosantilli.com/linux-kernel-module-cheat#getcpu */

#define _GNU_SOURCE
#include <assert.h>
#include <sched.h> /* getcpu */
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

/* man getcpu says this exists since glibc 2.29, but I can't find it in Ubuntu 20.04 glibc 2.31 not even with locate
 * (there is a hit under /usr/src/linux-headers-5.4.0-29/include/linux/getcpu.h but it only defines `struct getcpu_cache`
 * and nothing else:
 * https://stackoverflow.com/questions/23224607/how-do-i-include-linux-header-files-like-linux-getcpu-h
 * Furthermore, there is already a prototype in sched.h, so we can't define our own either. */
#if 0
#include <linux/getcpu.h>
#endif

void* main_thread(void *arg) {
    (void)arg;
    unsigned cpu, numa;
    getcpu(&cpu, &numa);
    printf("%u %u\n", cpu, numa);
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
