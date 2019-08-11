/* Let's see a trivial deadlock in action to feel the joys of multithreading.
 *
 * https://cirosantilli.com/linux-kernel-module-cheat#pthreads
 *
 * Exit successfully immediately without any arguments:
 *
 * ./pthread_deadlock.out
 *
 * Hang forever in a deadlock if one argument is given:
 *
 * ....
 * ./pthread_deadlock.out 0
 * ....
 */

#define _XOPEN_SOURCE 700
#include <assert.h>
#include <pthread.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    pthread_mutex_t main_thread_mutex = PTHREAD_MUTEX_INITIALIZER;
    (void)(argv);
    pthread_mutex_lock(&main_thread_mutex);
    if (argc > 1)
        pthread_mutex_lock(&main_thread_mutex);
    return EXIT_SUCCESS;
}
