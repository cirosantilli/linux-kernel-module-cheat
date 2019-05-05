/* https://github.com/cirosantilli/linux-kernel-module-cheat#bst-vs-heap */

#include <algorithm>
#include <iostream>
#include <queue>
#include <random>
#include <set>

#include "m5ops.h"

int main(int argc, char **argv) {
    typedef uint64_t I;
    std::vector<I> randoms;
    size_t i, n;
    std::priority_queue<I> heap;
    std::set<I> bst;
    unsigned int seed = std::random_device()();

    // CLI arguments.
    if (argc > 1) {
        n = std::stoi(argv[1]);
    } else {
        n = 1000;
    }

    // Action.
    for (i = 0; i < n; ++i) {
        randoms.push_back(i);
    }
    std::shuffle(randoms.begin(), randoms.end(), std::mt19937(seed));
    for (i = 0; i < n; ++i) {
        auto random = randoms[i];

        // Heap.
        m5_resetstats();
        heap.emplace(random);
        m5_dumpstats();

        // BST.
        m5_resetstats();
        bst.insert(random);
        m5_dumpstats();
    }
}
