// https://cirosantilli.com/linux-kernel-module-cheat#bst-vs-heap-vs-hashmap

#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#if LKMC_ALGORITHM_SET_STD_PRIORITY_QUEUE
#include <queue>
#endif
#include <random>
#if LKMC_ALGORITHM_SET_STD_SET
#include <set>
#endif
#if LKMC_ALGORITHM_SET_STD_UNORDERED_SET
#include <unordered_set>
#endif

#include <lkmc/m5ops.h>

int main(int argc, char **argv) {
    // Variables.
    typedef uint64_t T;
#if LKMC_ALGORITHM_SET_STD_PRIORITY_QUEUE
    std::priority_queue<T, std::vector<T>, std::greater<T>> set;
#endif
#if LKMC_ALGORITHM_SET_STD_SET
    std::set<T> set;
#endif
#if LKMC_ALGORITHM_SET_STD_UNORDERED_SET
    std::unordered_set<T> set;
#endif
    std::vector<T> input;
    size_t i, j = 0, n, granule, base;
#ifndef LKMC_M5OPS_ENABLE
    std::vector<std::chrono::nanoseconds::rep> dts;
    std::vector<decltype(base)> bases;
    using clk = std::chrono::high_resolution_clock;
    decltype(clk::now()) start, end;
#endif

    // CLI arguments.
    //
    // Save the clock time every `granule` loops.
    //
    // The magic value 0 means that a single time for the entire
    // run is printed, therefore accounting for the full run time.
    //
    // Otherwise, must be a divisor of the input size.
    //
    // Default value: 0
    if (argc > 1) {
        granule = std::stoll(argv[1]);
    } else {
        granule = 0;
    }

    // Read input from stdin.
    std::string str;
    while (std::getline(std::cin, str)) {
        if (str == "")
            break;
        input.push_back(std::stoll(str));
    }
    n = input.size();
    if (granule == 0) {
        granule = n;
    }

    // Action.
    for (i = 0; i < n / granule; ++i) {
        base = i * granule;
#ifndef LKMC_M5OPS_ENABLE
        start = clk::now();
#endif
        for (j = 0; j < granule; ++j) {
            LKMC_M5OPS_RESETSTATS;
#if LKMC_ALGORITHM_SET_STD_PRIORITY_QUEUE
            set.emplace(input[base + j]);
#else
            set.insert(input[base + j]);
#endif
            LKMC_M5OPS_DUMPSTATS;
        }
#ifndef LKMC_M5OPS_ENABLE
        end = clk::now();
        auto dt = (end - start) / granule;
        bases.push_back(base);
        dts.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(dt).count());
#endif
    }

    // Report results.
    std::cout << "output" << std::endl;
#if LKMC_ALGORITHM_SET_STD_PRIORITY_QUEUE
    // Print priority queue without duplicates.
    T last_val = set.top();
    std::cout << last_val << std::endl;
    set.pop();
    while (!set.empty()) {
        const auto& val = set.top();
        if (val != last_val)
            std::cout << val << std::endl;
        last_val = val;
        set.pop();
    }
#else
    for (const auto& item : set) {
        std::cout << item << std::endl;
    }
#endif
    std::cout << std::endl;
#ifndef LKMC_M5OPS_ENABLE
    std::cout << "times" << std::endl;
    auto bases_it = bases.begin();
    auto dts_it = dts.begin();
    while(bases_it != bases.end()) {
        const auto& base = *bases_it;
        const auto& dt = *dts_it;
        std::cout << base << " " << dt << std::endl;
        bases_it++;
        dts_it++;
    }
#endif
}
