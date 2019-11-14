// https://cirosantilli.com/linux-kernel-module-cheat#cpp-multithreading
//
// The non-atomic counters have undefined values which get printed:
// they are extremely likely to be less than the correct value due to
// race conditions on the data read and update of the ++.
//
// The atomic counters have defined values, and are asserted
//
// Atomic operations are more restricted than mutex as they can
// only protect a few operations on integers.
//
// But when they can be used, they can be much more efficient than mutees.
//
// On GCC 4.8 x86-64, using atomic offered a 5x peformance improvement
// over the same program with mutexes.

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
unsigned long global = 0;
#endif

#if defined(LKMC_USERLAND_ATOMIC_MUTEX)
std::mutex mutex;
#endif

#if defined(__x86_64__) || defined(__aarch64__)
unsigned long my_arch_atomic_ulong = 0;
unsigned long my_arch_non_atomic_ulong = 0;
#endif

void threadMain() {
    for (size_t i = 0; i < niters; ++i) {
#if defined(LKMC_USERLAND_ATOMIC_MUTEX)
        mutex.lock();
#endif
#if defined(LKMC_USERLAND_ATOMIC_FAIL) || \
    defined(LKMC_USERLAND_ATOMIC_STD_ATOMIC) || \
    defined(LKMC_USERLAND_ATOMIC_MUTEX)
        global++;
#endif
#if defined(LKMC_USERLAND_ATOMIC_MUTEX)
        mutex.unlock();
#endif
#if 0
#if defined(__x86_64__)
        __asm__ __volatile__ (
            "incq %0;"
            : "+m" (my_arch_non_atomic_ulong)
            :
            :
        );
        // https://cirosantilli.com/linux-kernel-module-cheat#x86-lock-prefix
        __asm__ __volatile__ (
            "lock;"
            "incq %0;"
            : "+m" (my_arch_atomic_ulong)
            :
            :
        );
#elif defined(__aarch64__)
        __asm__ __volatile__ (
            "add %0, %0, 1;"
            : "+r" (my_arch_non_atomic_ulong)
            :
            :
        );
        // https://cirosantilli.com/linux-kernel-module-cheat#arm-lse
        __asm__ __volatile__ (
            "ldadd %[inc], xzr, [%[addr]];"
            : "=m" (my_arch_atomic_ulong)
            : [inc] "r" (1),
              [addr] "r" (&my_arch_atomic_ulong)
            :
        );
#endif
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
    unsigned long expect = nthreads * niters;
#if defined(LKMC_USERLAND_ATOMIC_FAIL)
    std::cout << "expect " << expect << std::endl;
    std::cout << "global " << global << std::endl;
#elif defined(LKMC_USERLAND_ATOMIC_STD_ATOMIC)
    //assert(global.load() == expect);
    assert(global == expect);
#else
    // We can also use the atomics direclty through `operator T` conversion.
    assert(global == expect);
#endif
#if 0
#if defined(__x86_64__) || defined(__aarch64__)
    assert(my_arch_atomic_ulong == nthreads * niters);
    std::cout << "my_arch_non_atomic_ulong " << my_arch_non_atomic_ulong << std::endl;
#endif
#endif
#endif
}
