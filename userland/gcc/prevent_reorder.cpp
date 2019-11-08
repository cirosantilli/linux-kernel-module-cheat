/* https://cirosantilli.com/linux-kernel-module-cheat#compilers */

#include <chrono>
#include <iostream>
#include <string>

// noinline ensures that the ::now() cannot be split from the __asm__
template <class T>
__attribute__((noinline)) auto get_clock(T& value) {
    // Make the compiler think we actually use / modify the value.
    // It can't "see" what is going on inside the assembly string.
    __asm__ __volatile__ ("" : "+g" (value));
    return std::chrono::high_resolution_clock::now();
}

template <class T>
static T foo(T niters) {
    T result = 42;
    for (T i = 0; i < niters; ++i) {
        result = (result * result) - (3 * result) + 1;
    }
    return result;
}

int main(int argc, char **argv) {
    unsigned long long input;
    if (argc > 1) {
        input = std::stoull(argv[1], NULL, 0);
    } else {
        input = 1;
    }

    // Must come before because it could modify input
    // which is passed as a reference.
    auto t1 = get_clock(input);
    auto output = foo(input);
    // Must come after as it could use the output.
    auto t2 = get_clock(output);
    std::cout << "output " << output << std::endl;
    std::cout << "time (ns) "
              << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count()
              << std::endl;
}
