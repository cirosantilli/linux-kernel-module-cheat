// https://cirosantilli.com/linux-kernel-module-cheat#cpp-type-casting

#include <iostream>

struct B1 {
    B1(int int_in_b1) : int_in_b1(int_in_b1) {}
    virtual ~B1() {}
    void f0() {}
    virtual int f1() { return 1; }
    int int_in_b1;
};

struct B2 {
    B2(int int_in_b2) : int_in_b2(int_in_b2) {}
    virtual ~B2() {}
    virtual int f2() { return 2; }
    int int_in_b2;
};

struct D : public B1, public B2 {
    D(int int_in_b1, int int_in_b2, int int_in_d)
        : B1(int_in_b1), B2(int_in_b2), int_in_d(int_in_d) {}
    void d() {}
    int f2() { return 3; }
    int int_in_d;
};

int main() {
    B2 *b2s[2];
    B2 b2{11};
    D *dp;
    D d{1, 2, 3};

    // The memory layout must support the virtual method call use case.
    b2s[0] = &b2;
    // An upcast is an implicit static_cast<>().
    b2s[1] = &d;
    std::cout << "&d           " << &d           << std::endl;
    std::cout << "b2s[0]       " << b2s[0]       << std::endl;
    std::cout << "b2s[1]       " << b2s[1]       << std::endl;
    std::cout << "b2s[0]->f2() " << b2s[0]->f2() << std::endl;
    std::cout << "b2s[1]->f2() " << b2s[1]->f2() << std::endl;

    // Now for some downcasts.

    // Cannot be done implicitly
    // error: invalid conversion from ‘B2*’ to ‘D*’ [-fpermissive]
    // dp = (b2s[0]);

    // Undefined behaviour to an unrelated memory address because this is a B2, not D.
    dp = static_cast<D*>(b2s[0]);
    std::cout << "static_cast<D*>(b2s[0])            " << dp           << std::endl;
    std::cout << "static_cast<D*>(b2s[0])->int_in_d  " << dp->int_in_d << std::endl;

    // OK
    dp = static_cast<D*>(b2s[1]);
    std::cout << "static_cast<D*>(b2s[1])            " << dp           << std::endl;
    std::cout << "static_cast<D*>(b2s[1])->int_in_d  " << dp->int_in_d << std::endl;

    // Segfault because dp is nullptr.
    dp = dynamic_cast<D*>(b2s[0]);
    std::cout << "dynamic_cast<D*>(b2s[0])           " << dp           << std::endl;
    //std::cout << "dynamic_cast<D*>(b2s[0])->int_in_d " << dp->int_in_d << std::endl;

    // OK
    dp = dynamic_cast<D*>(b2s[1]);
    std::cout << "dynamic_cast<D*>(b2s[1])           " << dp           << std::endl;
    std::cout << "dynamic_cast<D*>(b2s[1])->int_in_d " << dp->int_in_d << std::endl;

    // Undefined behaviour to an unrelated memory address because this
    // did not calculate the offset to get from B2* to D*.
    dp = reinterpret_cast<D*>(b2s[1]);
    std::cout << "reinterpret_cast<D*>(b2s[1])           " << dp           << std::endl;
    std::cout << "reinterpret_cast<D*>(b2s[1])->int_in_d " << dp->int_in_d << std::endl;
}
