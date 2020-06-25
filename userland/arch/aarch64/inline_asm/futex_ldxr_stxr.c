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

#define LDXR_OPS_SIZE 1024
static int futex1 = 1;
static int futex2 = 1;
/* We do this to ensure that those two varibles are well separated.
 * If they are too close (same cache line?), then the str to ldxr_done
 * can make CPU1 lose the lock. */
static uint64_t ldxr_ops[LDXR_OPS_SIZE];
static uint64_t *ldxr_done = ldxr_ops;
static uint64_t *ldxr_var = ldxr_ops + LDXR_OPS_SIZE - 1;

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
        "ldxr x0, [%[ldxr_var]];mov %[ldxr_done], 1"
        : [ldxr_done] "=r" (*ldxr_done)
        : [ldxr_var] "r" (ldxr_var)
        : "x0", "memory"
    );
    lkmc_futex(&futex1, FUTEX_WAIT, futex1, NULL, NULL, 0);
    lkmc_futex(&futex2, FUTEX_WAIT, futex2, NULL, NULL, 0);
    return NULL;
}

int main(void) {
    pthread_t thread;
    pthread_create(&thread, NULL, thread_main, NULL);
    while (!*ldxr_done) {}
    /* Wait for thread1 to sleep on futex1. */
    busy_loop(1000, 1);
    /* Try to wake up the thread with an LLSC event.
     * It should not wake up in a correct implementation,
     * but it used to happen in gem5 before it was fixed. */
    __asm__ __volatile__ (
        "mov x0, 1;ldxr x0, [%0]; stxr w1, x0, [%0]"
        :
        : "r" (ldxr_var) : "x0", "x1", "memory"
    );
    /* Wait for thread1 to sleep on futex2. */
    busy_loop(1000, 1);
    /* Before it was fixed in gem5, this would wrongly wake a futex2
     * because the previous futex1 was woken up via LLSC. */
    lkmc_futex(&futex1, FUTEX_WAKE, 1, NULL, NULL, 0);
    assert(!pthread_join(thread, NULL));
}
