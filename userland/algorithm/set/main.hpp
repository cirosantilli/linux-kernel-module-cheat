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
    typedef uint64_t T;
#if LKMC_ALGORITHM_SET_STD_PRIORITY_QUEUE
    std::priority_queue<T, std::vector<T>, std::greater<int>> set;
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
#endif

    // CLI arguments.
    if (argc > 1) {
        granule = std::stoi(argv[1]);
    } else {
        granule = 1;
    }

    // Read input from stdin.
    std::string str;
    while (std::getline(std::cin, str)) {
        if (str == "")
            break;
        input.push_back(std::stoll(str));
    }
    n = input.size();

    // Action.
    for (i = 0; i < n / granule; ++i) {
#ifndef LKMC_M5OPS_ENABLE
        using clk = std::chrono::high_resolution_clock;
        decltype(clk::now()) start, end;
#endif
        base = i * granule;
#ifdef LKMC_M5OPS_ENABLE
        LKMC_M5OPS_RESETSTATS;
#else
        start = clk::now();
        for (j = 0; j < granule; ++j) {
#endif
#if LKMC_ALGORITHM_SET_STD_PRIORITY_QUEUE
        set.emplace(input[base + j]);
#else
        set.insert(input[base + j]);
#endif
#ifdef LKMC_M5OPS_ENABLE
        LKMC_M5OPS_DUMPSTATS;
#else
        }
        end = clk::now();
        auto dt = (end - start) / granule;
        bases.push_back(base);
        dts.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(dt).count());
#endif
    }

    // Report results.
    std::cout << "output" << std::endl;
#if LKMC_ALGORITHM_SET_STD_PRIORITY_QUEUE
    while (!set.empty()) {
        std::cout << set.top() << std::endl;
        set.pop();
    }
    //T last_val = set.top();
    //std::cout << last_val << std::endl;
    //set.pop();
    //while (!set.empty()) {
    //    const auto& val = set.top();
    //    if (val != last_val)
    //        std::cout << val << std::endl;
    //    last_val = val;
    //    set.pop();
    //}
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
