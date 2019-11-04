// https://cirosantilli.com/linux-kernel-module-cheat#cpp-multithreading
//
// Spawn some threads and print their ID.
//
// On Ubuntu 19.04, they ar large possibly non-consecutive numbers.

#include <cstring>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

std::mutex mutex;

void myfunc(int i) {
    // Mutex and flush to prevent the output from
    // different threads from interleaving.
    mutex.lock();
    std::cout <<
        i << " " <<
        std::this_thread::get_id() << std::endl
        << std::flush;
    mutex.unlock();
}

int main(int argc, char **argv) {
    std::cout << "main " << std::this_thread::get_id() << std::endl;
    std::vector<std::thread> threads;
    unsigned int nthreads;

    // CLI arguments.
    if (argc > 1) {
        nthreads = std::strtoll(argv[1], NULL, 0);
    } else {
        nthreads = 1;
    }

    // Action.
    for (unsigned int i = 0; i < nthreads; ++i) {
        threads.push_back(std::thread(myfunc, i));
    }
    for (auto& thread : threads) {
        thread.join();
    }
}
