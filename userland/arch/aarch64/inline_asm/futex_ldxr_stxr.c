/* https://cirosantilli.com/linux-kernel-module-cheat#futex-system-call
 * https://cirosantilli.com/linux-kernel-module-cheat#arm-wfe-global-monitor-events */

#define _GNU_SOURCE
#include <assert.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>

#include <lkmc/futex.h>

static int futex1 = 1;
static int futex2 = 1;
atomic_int ldxr_done = 0;
atomic_int stdr_wake_done = 0;
static uint64_t ldxr_var = 0;

void __attribute__ ((noinline)) busy_loop(
    unsigned long long max,
    unsigned long long max2
) {
    for (unsigned long long i = 0; i < max2; i++) {
        for (unsigned long long j = 0; j < max; j++) {
            __asm__ __volatile__ ("" : "+g" (i), "+g" (j) : :);
        }
    }
}

void* thread_main(void *arg) {
    (void)arg;
    __asm__ __volatile__ (
        "ldxr x0, [%0]"
        :
        : "r" (&ldxr_var) : "x0", "memory"
    );
    ldxr_done = 1;
    lkmc_futex(&futex1, FUTEX_WAIT, futex1, NULL, NULL, 0);
    lkmc_futex(&futex2, FUTEX_WAIT, futex2, NULL, NULL, 0);
    return NULL;
}

int main(void) {
    pthread_t thread;
    pthread_create(&thread, NULL, thread_main, NULL);
    while (!ldxr_done) {}
    /* Wait for thread1 to sleep on futex1. */
    busy_loop(1000, 1);
    /* Wrongly wake up the thread with a SEV. */
    __asm__ __volatile__ (
        "mov x0, 1;ldxr x0, [%0]; stxr w1, x0, [%0]"
        :
        : "r" (&ldxr_var) : "x0", "x1", "memory"
    );
    /* Wait for thread1 to sleep on futex2. */
    busy_loop(1000, 1);
    /* Wrongly wake thread from futex1 again. */
    /* But it is now sleeping on futex2, so this is wrong. */
    lkmc_futex(&futex1, FUTEX_WAKE, 1, NULL, NULL, 0);
    assert(!pthread_join(thread, NULL));
}
