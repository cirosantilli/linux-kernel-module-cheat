// https://cirosantilli.com/linux-kernel-module-cheat#cpp-decltype

#include <cassert>
#include <vector>
#include <utility> // declval

int f() {
    return 1;
}

class C {
    public:
        int f() { return 2; }
};

int i;
decltype(i) g() {
    return 1;
}

int main() {
#if __cplusplus >= 201103L
    // Implies reference while auto does not.
    {
        int i = 0;
        int& ir = i;
        decltype(ir) ir2 = ir;
        ir2 = 1;
        assert(i == 1);
    }

    // Can be used basically anywhere.
    {
        int i = 0;
        std::vector<decltype(i)> v;
        v.push_back(0);
    }

    // Return value.
    {
        decltype(f()) i;
        assert(typeid(i) == typeid(int));

        C c;
        decltype(c.f()) j;
        assert(typeid(j) == typeid(int));

        // Return value without instance. Use declval.
        // http://stackoverflow.com/questions/9760358/decltype-requires-instantiated-object
        decltype(std::declval<C>().f()) k;
        assert(typeid(k) == typeid(int));
    }

    // decltype must take expressions as input, not a type.
    // For types with the default constructor like `int`, we can just call the default constructor as in int():
    // https://stackoverflow.com/questions/39279074/what-does-the-void-in-decltypevoid-mean-exactly
    // or we can just use declval.
    {
        decltype(int()) i = 0;
        assert(typeid(i) == typeid(int));

        decltype(std::declval<int>()) j = 0;
        assert(typeid(j) == typeid(int));
    }

    // Can be used to declare the return value of functions.
    assert(g() == 1);
#endif
}
