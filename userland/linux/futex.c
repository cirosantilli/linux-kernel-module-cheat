/* https://cirosantilli.com/linux-kernel-module-cheat#futex-system-call */

#define _GNU_SOURCE
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <lkmc/futex.h>

static int sleep_done;

void* main_thread(void *arg) {
    (void)arg;
    puts("child start");
    /* We only sleep if sleep_done is 0. In that case, we are certain
     * that a FUTEX_WAKE is coming later on, so we won't deadlock.
     *
     * The while is needed due to the possibility of spurious wakeups.
     *
     * The __atomic_load_n is not enough to ensure that we only sleep
     * if sleep_done is 0, because a sleep_done could happen after the check:
     * this is guaranteed by the system call itself which atomically checks that
     * for us as well.
     */
    while (!__atomic_load_n(&sleep_done, __ATOMIC_ACQUIRE)) {
        lkmc_futex(&sleep_done, FUTEX_WAIT, 0, NULL, NULL, 0);
    }
    puts("child after parent sleep");
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
    sleep(1);
    puts("parent after sleep");
    /* Mark the sleep as done. If the child still didn't reach the FUTEX_WAIT for some miracle,
     * (extremely unlikely because we've just slept for one entire second)
     * we don't want it to sleep, otherwise the FUTEX_WAKE that we will be done next might happen
     * before the FUTEX_WAIT, which would have no effect, and so FUTEX_WAIT would sleep forever.
     *
     * __ATOMIC_ACQUIRE and __ATOMIC_RELEASE are enough here: together they ensure that once this
     * store is done, then the load MUST see it, which is exactly what we need to avoid deadlocks.
     * TODO is RELAXED enough? Why yes/not?
     * See also: https://cirosantilli.com/linux-kernel-module-cheat#cpp-memory-order
     */
    __atomic_store_n(&sleep_done, 1, __ATOMIC_RELEASE);
    lkmc_futex(&sleep_done, FUTEX_WAKE, 1, NULL, NULL, 0);
    assert(!pthread_join(thread, NULL));
}
