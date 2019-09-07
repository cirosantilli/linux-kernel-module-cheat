// https://cirosantilli.com/linux-kernel-module-cheat#cpp

#if __cplusplus >= 201703L
#include <cassert>
#include <type_traits>

template <class T>
struct MyClass {
    int myFunc() {
        if constexpr(std::is_integral<T>())
            return 1;
        else
            return 2;
    }
};
#endif

int main() {
#if __cplusplus >= 201703L
    assert(MyClass<int>().myFunc() == 1);
    assert(MyClass<float>().myFunc() == 2);
#endif
}
