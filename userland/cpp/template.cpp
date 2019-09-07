// https://cirosantilli.com/linux-kernel-module-cheat#cpp

#include <cassert>

template <class T>
struct MyClass {
    T myVal;
    MyClass(T myVal) : myVal(myVal) {}
    T myFunc() {
        return myVal + 1;
    }
};

int main() {
    assert(MyClass<int>(1).myFunc() == 2);
    assert(MyClass<float>(1.5).myFunc() == 2.5);
}
