// https://cirosantilli.com/linux-kernel-module-cheat#arm-wfe-global-monitor-events

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>

#include <lkmc/futex.h>

std::atomic_ulong done;
int futex = 1;

void myfunc() {
    __asm__ __volatile__ ("sevl;wfe;ldxr x0, [%0];wfe" : : "r" (&futex) : "x0");
    done.store(futex);
}

int main(int argc, char **argv) {
    bool do_sev = true;
    if (argc > 1) {
        do_sev = (argv[1][0] != '0');
    }
    done.store(0);
    std::thread thread;
    thread = std::thread(myfunc);
    while (!done.load()) {
        if (do_sev) {
            __asm__ __volatile__ ("ldxr x0, [%0];mov x0, 1;stxr w1, x0, [%0]" : : "r" (&futex) : "x0", "x1");
        }
    }
    thread.join();
}
