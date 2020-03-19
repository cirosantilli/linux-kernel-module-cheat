// https://cirosantilli.com/linux-kernel-module-cheat#cpp

int main() {

    struct D {
        int i;
        constexpr D() : i(0) {}
        constexpr D(int i) : i(i) {}
    };

    struct C {
        int i;
        constexpr C() : i(1) {}
        constexpr C(int i) : i(i) {}
        constexpr C(const D& d) : i(d.i) {}
    };

    // Declares *FUNCTION* called `c` that returns `C` inside function main.
    //
    // This is the same as in C, where it is possible to declare a function from inside another function,
    // but not define it.
    //
    // Therefore there would be not way for C++ to distinguish between the two,
    // and still be backwards compatible with C.
    {
        constexpr C c();

#if 0
        // ERROR: function definition is not possible inside another function.
        C c() {return C();}
#endif

        //c.i;
    }

    // If you want to call a default constructor, use:
    {
        constexpr C c;
        static_assert(c.i == 1);
    }

    // For non-default constructors, literal arguments disambiguate
    // things as this syntax could not possibly be a function declaration.
    {
        constexpr C c(2);
        static_assert(c.i == 2);
    }

    // But sometimes even arguments are not enough: here D()
    // is interpreted as "a function of type `D f()`"
    {
        constexpr C c(D());
#if 0
        // error: request for member ‘i’ in ‘c’, which is of non-class type ‘main()::C(main()::D (*)())’
        static_assert(c.i == 0);
#endif
    }

    // Solving the most vexing parse.
    // https://stackoverflow.com/questions/13249694/avoid-the-most-vexing-parse
    {
        // Extra parenthesis.
        {
            constexpr C c((D(2)));
            static_assert(c.i == 2);
        }

        // Initialize through assignment. TODO likely guaranteed to be cost-free,
        // but confirm.
        {
            constexpr C c = C((D(2)));
            static_assert(c.i == 2);
        }

        // Initializer list. Only works if there is no initializer_list constructor.
        // Only works in general if c does not have an ambiguous initializer_list constructor though.
        {
            constexpr C c{D(2)};
            static_assert(c.i == 2);
        }
    }
}
