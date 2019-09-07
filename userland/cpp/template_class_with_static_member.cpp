// https://cirosantilli.com/linux-kernel-module-cheat#cpp

#include <cassert>

template <class T>
struct MyClass {
    static int i;
    MyClass() {
        i++;
    }
};

template <class T>
int MyClass<T>::i = 0;

int main() {
    MyClass<int>();
    MyClass<int>();
    assert(MyClass<int>::i == 2);
}
