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
    size_t i, j, n, granule, base;
    std::priority_queue<I> heap;
    std::set<I> bst;
    std::unordered_set<I> hashmap;
    unsigned int seed = std::random_device()();

    // CLI arguments.
    if (argc > 1) {
        n = std::stoi(argv[1]);
    } else {
        n = 10;
    }
#ifdef LKMC_M5OPS_ENABLE
    // Let's comment useless stuff out to speed up gem5 simulations.
    granule = 1;
    j = 0;
#else
    if (argc > 2) {
        granule = std::stoi(argv[2]);
    } else {
        granule = 1;
    }
#endif

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
#ifndef LKMC_M5OPS_ENABLE
        start = clk::now();
        for (j = 0; j < granule; ++j) {
#endif
            LKMC_M5OPS_RESETSTATS;
            heap.emplace(randoms[base + j]);
            LKMC_M5OPS_DUMPSTATS;
#ifndef LKMC_M5OPS_ENABLE
        }
        end = clk::now();
        auto dt_heap = (end - start) / granule;
#endif

        // BST.
#ifndef LKMC_M5OPS_ENABLE
        start = clk::now();
        for (j = 0; j < granule; ++j) {
#endif
            LKMC_M5OPS_RESETSTATS;
            bst.insert(randoms[base + j]);
            LKMC_M5OPS_DUMPSTATS;
#ifndef LKMC_M5OPS_ENABLE
        }
        end = clk::now();
        auto dt_bst = (end - start) / granule;
#endif

        // Hashmap.
#ifndef LKMC_M5OPS_ENABLE
        start = clk::now();
        for (j = 0; j < granule; ++j) {
#endif
            LKMC_M5OPS_RESETSTATS;
            hashmap.insert(randoms[base + j]);
            LKMC_M5OPS_DUMPSTATS;
#ifndef LKMC_M5OPS_ENABLE
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
