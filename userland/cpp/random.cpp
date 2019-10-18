// https://cirosantilli.com/linux-kernel-module-cheat#cpp

#include <iostream>
#include <random>

int main(int argc, char **argv) {
    // Potentially expensive /dev/urandom read that waits for entropy.
    std::random_device dev;
    std::mt19937 prng(dev());
    std::uniform_int_distribution<uint64_t> zero_ull_max(0);
    std::uniform_int_distribution<uint8_t> one_to_six(1, 6);

    unsigned long long loops = 1;
    if (argc > 1) {
        loops = std::stoull(argv[1], NULL, 0);
    }

    for (auto i = 0U; i < loops; ++i) {
        std::cout << "zero_to_ull_max 0x" << std::hex << zero_ull_max(prng) << std::endl;
        std::cout << "one_to_six " << std::dec << one_to_six(prng) << std::endl;
    }
}
