// https://cirosantilli.com/linux-kernel-module-cheat#cpp

#include <lkmc.h>

#include <cassert>
#include <fstream>
#include <sstream>

// https://stackoverflow.com/questions/116038/what-is-the-best-way-to-read-an-entire-file-into-a-stdstring-in-c
// https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
std::string read_file(const std::string& path) {
    std::ifstream ifs(path);
    assert(ifs.is_open());
    std::stringstream sstr;
    sstr << ifs.rdbuf();
    return sstr.str();
}

int main(void) {
    std::string path = LKMC_TMP_FILE;
    std::string data = "asdf\nqwer\n";

    // Write entire string to file at once.
    {
        std::ofstream ofs(path);
        assert(ofs.is_open());
        ofs << data;
        ofs.close();
    }

    // Read entire file into string.
    std::string read_output = read_file(path);
    assert(read_output == data);

    // Append to a file.
    {
        std::string append_data = "zxcv\n";
        std::ofstream ofs(path, std::ios::app);
        assert(ofs.is_open());
        ofs << append_data;
        ofs.close();
        assert(read_file(path) == data + append_data);
    }

    return EXIT_SUCCESS;
}
