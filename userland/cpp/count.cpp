// Count to infinity sleeping one second per number.
//
// https://github.com/cirosantilli/linux-kernel-module-cheat#cpp-multithreading

#include <chrono>
#include <thread>
#include <iostream>

int main() {
    int i = 0;
    while (1) {
        std::cout << i << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        i++;
    }
}
