// https://cirosantilli.com/linux-kernel-module-cheat#cpp
//
// ./build disabled for now because of GCC std::filesystem mess.
// Compile manually with:
// g++-8 -std=c++17 -o temporary_directory.out temporary_directory.cpp -lstdc++fs

#if __cplusplus >= 201703L
#include <exception>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>

#include <filesystem>

std::filesystem::path create_temporary_directory(unsigned long long max_tries = 100) {
    auto tmp_dir = std::filesystem::temp_directory_path();
    unsigned long long i = 0;
    std::random_device dev;
    std::mt19937 prng(dev());
    std::uniform_int_distribution<uint64_t> rand(0);
    std::filesystem::path path;
    while (true) {
        std::stringstream ss;
        ss << std::hex << rand(prng);
        path = tmp_dir / ss.str();
        // true if the directory was created.
        if (std::filesystem::create_directory(path)) {
            break;
        }
        if (i == max_tries) {
            throw std::runtime_error("could not find non-existing directory");
        }
        i++;
    }
    return path;
}

#endif

int main() {
#if __cplusplus >= 201703L
    std::cout << "temp_directory_path() = "
              << std::filesystem::temp_directory_path()
              << std::endl;

    auto tmpdir = create_temporary_directory();
    std::cout << "create_temporary_directory() = "
              << tmpdir
              << std::endl;


    // Use our temporary directory.
    std::ofstream ofs(tmpdir / "myfile");
    ofs << "asdf\nqwer\n";
    ofs.close();

    // Remove the directory and its contents.
    std::filesystem::remove_all(tmpdir);
#endif
}
