// https://cirosantilli.com/linux-kernel-module-cheat#cpp


#if __cplusplus >= 201703L
#include <cassert>
#include <type_traits>

template<typename T>
struct MyClass {
    MyClass() : myVar{0} {}
    void modifyIfNotConst() {
        if constexpr(!isconst) {
            myVar = 1;
        }
    }
    T myVar;
    static constexpr bool isconst = std::is_const<T>::value;
};
#endif

int main() {
#if __cplusplus >= 201703L
    MyClass<double> x;
    MyClass<const double> y;
    x.modifyIfNotConst();
    y.modifyIfNotConst();
    assert(x.myVar == 1);
    assert(y.myVar == 0);
#endif
}
