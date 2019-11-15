// https://cirosantilli.com/linux-kernel-module-cheat#atomic-cpp

#if __cplusplus >= 201103L
#include <atomic>
#include <cassert>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

size_t niters;

#if defined(LKMC_USERLAND_ATOMIC_STD_ATOMIC)
std::atomic_ulong global(0);
#else
uint64_t global = 0;
#endif

#if defined(LKMC_USERLAND_ATOMIC_MUTEX)
std::mutex mutex;
#endif

void threadMain() {
    for (size_t i = 0; i < niters; ++i) {
#if defined(LKMC_USERLAND_ATOMIC_MUTEX)
        mutex.lock();
#endif
#if defined(LKMC_USERLAND_ATOMIC_X86_64_INC)
        __asm__ __volatile__ (
            "incq %0;"
            : "+g" (global)
            :
            :
        );
#elif defined(LKMC_USERLAND_ATOMIC_X86_64_LOCK_INC)
        // https://cirosantilli.com/linux-kernel-module-cheat#x86-lock-prefix
        __asm__ __volatile__ (
            "lock;"
            "incq %0;"
            : "+m" (global)
            :
            :
        );
#elif defined(LKMC_USERLAND_ATOMIC_AARCH64_ADD)
        __asm__ __volatile__ (
            "add %0, %0, 1;"
            : "+r" (global)
            :
            :
        );
#elif defined(LKMC_USERLAND_ATOMIC_AARCH64_LDADD)
        // https://cirosantilli.com/linux-kernel-module-cheat#arm-lse
        __asm__ __volatile__ (
            "ldadd %[inc], xzr, [%[addr]];"
            : "=m" (global)
            : [inc] "r" (1),
              [addr] "r" (&global)
            :
        );
#else
        global++;
#endif
#if defined(LKMC_USERLAND_ATOMIC_MUTEX)
        mutex.unlock();
#endif
#if 0
#endif
    }
}
#endif

int main(int argc, char **argv) {
#if __cplusplus >= 201103L
    size_t nthreads;
    if (argc > 1) {
        nthreads = std::stoull(argv[1], NULL, 0);
    } else {
        nthreads = 2;
    }
    if (argc > 2) {
        niters = std::stoull(argv[2], NULL, 0);
    } else {
        niters = 10;
    }
    std::vector<std::thread> threads(nthreads);
    for (size_t i = 0; i < nthreads; ++i)
        threads[i] = std::thread(threadMain);
    for (size_t i = 0; i < nthreads; ++i)
        threads[i].join();
    uint64_t expect = nthreads * niters;
#if defined(LKMC_USERLAND_ATOMIC_FAIL) || \
    defined(LKMC_USERLAND_ATOMIC_X86_64_INC) || \
    defined(LKMC_USERLAND_ATOMIC_AARCH64_INC)
    // These fail, so we just print the outcomes.
    std::cout << "expect " << expect << std::endl;
    std::cout << "global " << global << std::endl;
#else
    assert(global == expect);
#endif
#endif
}
