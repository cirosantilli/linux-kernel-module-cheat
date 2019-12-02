// https://cirosantilli.com/linux-kernel-module-cheat#cpp-parallel-algorithms

#include <algorithm>
#include <cassert>
#include <chrono>
#include <execution>
#include <random>
#include <iostream>
#include <vector>

int main(int argc, char **argv) {
    using clk = std::chrono::high_resolution_clock;
    decltype(clk::now()) start, end;
    std::vector<unsigned long long> input_parallel, input_serial;
    unsigned int seed;
    unsigned long long n;

    // CLI arguments;
    std::uniform_int_distribution<uint64_t> zero_ull_max(0);
    if (argc > 1) {
        n = std::strtoll(argv[1], NULL, 0);
    } else {
        n = 10;
    }
    if (argc > 2) {
        seed = std::stoi(argv[2]);
    } else {
        seed = std::random_device()();
    }

    // Calculate random inpu.
    std::mt19937 prng(seed);
    for (unsigned long long i = 0; i < n; ++i) {
        input_parallel.push_back(zero_ull_max(prng));
    }
    input_serial = input_parallel;

    // Sort and time parallel.
    start = clk::now();
    std::sort(std::execution::par_unseq, input_parallel.begin(), input_parallel.end());
    end = clk::now();
    std::cout << "parallel " << std::chrono::duration<float>(end - start).count() << " s" << std::endl;

    // Sort and time serial.
    start = clk::now();
    std::sort(std::execution::seq, input_serial.begin(), input_serial.end());
    end = clk::now();
    std::cout << "serial   " << std::chrono::duration<float>(end - start).count() << " s" << std::endl;

    assert(input_parallel == input_serial);
}
