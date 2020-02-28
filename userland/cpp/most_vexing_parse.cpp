// https://cirosantilli.com/linux-kernel-module-cheat#cpp

#include <cassert>

int main() {

    struct D {
        int i;
        D() {}
        D(int i) : i(i) {}
    };

    struct C {
        int i;
        C() : i(1) {}
        C(int i) : i(i) {}
        C(const D& d) : i(d.i) {}
    };

    // Declares *FUNCTION* called `c` that returns `C` inside function main.
    //
    // This is the same as in C, where it is possible to declare a function from inside another function,
    // but not define it.
    //
    // Therefore there would be not way for C++ to distinguish between the two,
    // and still be backwards compatible with C.
    {
        C c();

#if 0
        // ERROR: function definition is not possible inside another function.
        C c() {return C();}
#endif

        //c.i;
    }

    // If you want to call a default constructor, use:
    {
        C c;
        assert(c.i == 1);
    }

    // For non-default constructors, literal arguments disambiguate
    // things as this syntax could not possibly be a function declaration.
    {
        C c(2);
        assert(c.i == 2);
    }

    // But sometimes even arguments are not enough: here D()
    // is interpreted as "a function of type `D f()`"
    {
        C c(D(2));
#if 0
        // error: request for member ‘i’ in ‘c’, which is of non-class type ‘main()::C(main()::D (*)())’
        assert(c.i == 2);
#endif
    }

    // Solving the most vexing parse.
    // https://stackoverflow.com/questions/13249694/avoid-the-most-vexing-parse
    {
        // Extra parenthesis.
        {
            C c((D(2)));
            assert(c.i == 2);
        }

        // Initialize through assignment. TODO likely guaranteed to be cost-free,
        // but confirm.
        {
            C c = C((D(2)));
            assert(c.i == 2);
        }

        // Initializer list. Only works if there is no initializer_list constructor.
        // Only works in general if c does not have an ambiguous initializer_list constructor though.
        {
            C c{D(2)};
            assert(c.i == 2);
        }
    }
}
