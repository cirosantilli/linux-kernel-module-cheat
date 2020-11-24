// https://cirosantilli.com/linux-kernel-module-cheat#cpp-rule-of-five
// Adapted from https://en.cppreference.com/w/cpp/language/rule_of_five

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <utility>

class rule_of_five
{
    char* cstring;
 public:
    rule_of_five(const char* s = "")
    : cstring(nullptr) {
        std::cout << "constructor" << std::endl;
        if (s) {
            std::size_t n = std::strlen(s) + 1;
            cstring = new char[n];
            std::memcpy(cstring, s, n);
        }
    }

    ~rule_of_five() {
        std::cout << "destructor" << std::endl;
        delete[] cstring;
    }

    rule_of_five(const rule_of_five& other)
    : rule_of_five(other.cstring) {
        std::cout << "copy" << std::endl;
    }

    rule_of_five& operator=(const rule_of_five& other) {
        std::cout << "copy assignment" << std::endl;
        return *this = rule_of_five(other);
    }

    rule_of_five(rule_of_five&& other) noexcept
    : cstring(std::exchange(other.cstring, nullptr)) {
        std::cout << "move" << std::endl;
    }

    rule_of_five& operator=(rule_of_five&& other) noexcept {
        std::cout << "move assignment" << std::endl;
        std::swap(cstring, other.cstring);
        return *this;
    }
};

int main()
{
    std::cout << "constructor?" << std::endl;
    rule_of_five o1{"aaa"};
    std::cout << std::endl;

    std::cout << "copy?" << std::endl;
    auto o2{o1};
    std::cout << std::endl;

    std::cout << "copy assignment?" << std::endl;
    rule_of_five o3("bbb");
    o3 = o2;
    std::cout << std::endl;

    std::cout << "move?" << std::endl;
    rule_of_five o4(rule_of_five("ccc"));
    std::cout << std::endl;

    std::cout << "move?" << std::endl;
    rule_of_five o5("ddd");
    o5 = rule_of_five("eee");
    std::cout << std::endl;

    std::cout << "a bunch of destructors?" << std::endl;
}
