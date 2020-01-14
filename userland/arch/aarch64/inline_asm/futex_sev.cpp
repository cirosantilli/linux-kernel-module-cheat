// https://cirosantilli.com/linux-kernel-module-cheat#wfe-from-userland

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
    lkmc_futex(&futex, FUTEX_WAIT, futex, NULL, NULL, 0);
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
            __asm__ __volatile__ ("sev");
        }
    }
    thread.join();
}
