// https://cirosantilli.com/linux-kernel-module-cheat#arm-wfe-and-sev-instructions

#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>

std::atomic_ulong done;

void myfunc() {
    unsigned long new_val = 1;
    __asm__ __volatile__ ("wfe;wfe;" : "+r" (new_val) : :);
    done.store(new_val);
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
        if (do_sev)
            __asm__ __volatile__ ("sev");
    }
    thread.join();
}
