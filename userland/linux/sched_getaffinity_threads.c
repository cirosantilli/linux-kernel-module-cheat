/* https://cirosantilli.com/linux-kernel-module-cheat#gdb-step-debug-multicore-userland */

#define _GNU_SOURCE
#include <assert.h>
#include <pthread.h>
#include <sched.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void* main_thread_0(void *arg) {
    int i;
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(*((int*)arg), &mask);
    sched_setaffinity(0, sizeof(cpu_set_t), &mask);
    i = 0;
    while (true) {
        printf("0 %d\n", i);
        sleep(1);
        i++;
    }
    return NULL;
}

void* main_thread_1(void *arg) {
    int i;
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(*((int*)arg), &mask);
    sched_setaffinity(1, sizeof(cpu_set_t), &mask);
    i = 0;
    while (true) {
        printf("1 %d\n", i);
        sleep(1);
        i++;
    }
    return NULL;
}

int main(void) {
    enum NUM_THREADS {NUM_THREADS = 2};
    pthread_t threads[NUM_THREADS];
    int thread_args[NUM_THREADS];
    pthread_create(&threads[0], NULL, main_thread_0, (void*)&thread_args[0]);
    pthread_create(&threads[1], NULL, main_thread_1, (void*)&thread_args[1]);
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    return EXIT_SUCCESS;
}
