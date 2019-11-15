// https://cirosantilli.com/linux-kernel-module-cheat#bst-vs-heap-vs-hashmap

#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <queue>
#include <random>
#include <set>
#include <unordered_set>

#include <lkmc/m5ops.h>

int main(int argc, char **argv) {
    typedef uint64_t I;
    std::vector<I> randoms;
    size_t i, n, granule, base;
    std::priority_queue<I> heap;
    std::set<I> bst;
    std::unordered_set<I> hashmap;
    unsigned int seed;
    size_t j = 0;

    // CLI arguments.
    if (argc > 1) {
        n = std::stoi(argv[1]);
    } else {
        n = 10;
    }
    if (argc > 2) {
        granule = std::stoi(argv[2]);
    } else {
        granule = 1;
    }
    if (argc > 3) {
        seed = std::stoi(argv[3]);
    } else {
        seed = std::random_device()();
    }

    // Action.
    for (i = 0; i < n; ++i) {
        randoms.push_back(i);
    }
    std::shuffle(randoms.begin(), randoms.end(), std::mt19937(seed));
    for (i = 0; i < n / granule; ++i) {
#ifndef LKMC_M5OPS_ENABLE
        using clk = std::chrono::high_resolution_clock;
        decltype(clk::now()) start, end;
#endif
        base = i * granule;

        // Heap.
#ifdef LKMC_M5OPS_ENABLE
        LKMC_M5OPS_RESETSTATS;
#else
        start = clk::now();
        for (j = 0; j < granule; ++j) {
#endif
        heap.emplace(randoms[base + j]);
#ifdef LKMC_M5OPS_ENABLE
        LKMC_M5OPS_DUMPSTATS;
#else
        }
        end = clk::now();
        auto dt_heap = (end - start) / granule;
#endif

        // BST.
#ifdef LKMC_M5OPS_ENABLE
        LKMC_M5OPS_RESETSTATS;
#else
        start = clk::now();
        for (j = 0; j < granule; ++j) {
#endif
        bst.insert(randoms[base + j]);
#ifdef LKMC_M5OPS_ENABLE
        LKMC_M5OPS_DUMPSTATS;
#else
        }
        end = clk::now();
        auto dt_bst = (end - start) / granule;
#endif

        // Hashmap.
#ifdef LKMC_M5OPS_ENABLE
        LKMC_M5OPS_RESETSTATS;
#else
        start = clk::now();
        for (j = 0; j < granule; ++j) {
#endif
        hashmap.insert(randoms[base + j]);
#ifdef LKMC_M5OPS_ENABLE
        LKMC_M5OPS_DUMPSTATS;
#else
        }
        end = clk::now();
        auto dt_hashmap = (end - start) / granule;
#endif

#ifndef LKMC_M5OPS_ENABLE
        // Output.
        std::cout
            << base << " "
            << std::chrono::duration_cast<std::chrono::nanoseconds>(dt_heap).count() << " "
            << std::chrono::duration_cast<std::chrono::nanoseconds>(dt_bst).count() << " "
            << std::chrono::duration_cast<std::chrono::nanoseconds>(dt_hashmap).count() << std::endl
        ;
#endif
    }

    // Sanity check.
    for (auto it = bst.rbegin(); it != bst.rend(); ++it) {
        assert(*it == heap.top());
        heap.pop();
    }
}
