// http://stackoverflow.com/questions/150355/programmatically-find-the-number-of-cores-on-a-machine
//
// Not affected by taskset: https://stackoverflow.com/questions/1006289/how-to-find-out-the-number-of-cpus-using-python/55423170#55423170

#include <iostream>
#include <thread>

int main() {
    std::cout << std::thread::hardware_concurrency() << std::endl;
}
