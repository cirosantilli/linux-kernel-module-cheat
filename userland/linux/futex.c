/* https://cirosantilli.com/linux-kernel-module-cheat#futex-system-call */

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <lkmc/futex.h>

static int futex1, futex2;

void* main_thread(void *arg) {
    (void)arg;
    puts("child start");
    lkmc_futex(&futex1, FUTEX_WAKE, 1, NULL, NULL, 0);
    lkmc_futex(&futex2, FUTEX_WAIT, 0, NULL, NULL, 0);
    puts("child end");
    return NULL;
}

int main(void) {
    pthread_t thread;

    puts("main start");
    assert(!pthread_create(
        &thread,
        NULL,
        main_thread,
        NULL
    ));
    lkmc_futex(&futex1, FUTEX_WAIT, 0, NULL, NULL, 0);
    sleep(1);
    puts("main after sleep");
    lkmc_futex(&futex2, FUTEX_WAKE, 1, NULL, NULL, 0);
    assert(!pthread_join(thread, NULL));
}
