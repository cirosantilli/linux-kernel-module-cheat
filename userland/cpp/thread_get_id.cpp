// https://cirosantilli.com/linux-kernel-module-cheat#cpp-multithreading
//
// Spawn some threads and print their ID.
//
// On Ubuntu 19.04, they ar large possibly non-consecutive numbers.

#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

std::mutex mutex;

void myfunc(int i) {
    mutex.lock();
    std::cout << i << " " << std::this_thread::get_id() << std::endl;
    mutex.unlock();
}

int main() {
    std::cout << "main " << std::this_thread::get_id() << std::endl;
    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < 4; ++i) {
        threads.push_back(std::thread(myfunc, i));
    }
    for (auto& thread : threads) {
        thread.join();
    }
}
