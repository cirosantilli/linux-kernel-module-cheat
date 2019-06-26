// https://github.com/cirosantilli/linux-kernel-module-cheat#atomic
//
// More restricted than mutex as it can only protect a few operations on integers.
//
// But if that is the use case, may be more efficient.
//
// On GCC 4.8 x86-64, using atomic is a huge peformance improvement
// over the same program with mutexes (5x).

#include <atomic>
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>

#if __cplusplus >= 201103L
std::atomic_ulong my_atomic_ulong(0);
unsigned long my_non_atomic_ulong = 0;
size_t niters;

void threadMain() {
    for (size_t i = 0; i < niters; ++i) {
        my_atomic_ulong++;
        my_non_atomic_ulong++;
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
        niters = 1000;
    }
    std::vector<std::thread> threads(nthreads);
    for (size_t i = 0; i < nthreads; ++i)
        threads[i] = std::thread(threadMain);
    for (size_t i = 0; i < nthreads; ++i)
        threads[i].join();
    assert(my_atomic_ulong.load() == nthreads * niters);
    // Same as above through `operator T`.
    assert(my_atomic_ulong == nthreads * niters);
    std::cout << my_non_atomic_ulong << std::endl;
#endif
}
