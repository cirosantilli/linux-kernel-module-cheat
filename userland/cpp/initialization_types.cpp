// https://cirosantilli.com/linux-kernel-module-cheat#cpp-initialization-types

#include <cassert>
#include <type_traits>

// zero-initialization because has static storage
// just like a static "local" function variable.
int global;

int main() {

    // First let's list how different initializations are done explicitly in the code.
    {
        struct C {
            int i;
            constexpr C() : i(0) {}
            constexpr C(int i) : i(i) {}
        };

        // Default-initialization.
        {
            C i;
            C *j = new C;
        }

        // Value initialization.
        {
            C i{};
            C j = C();
            C k = C{};
            C *l = new C();
            C *m = new C{};
        }

        // Direct initialization.
        {
            C i(1);
            C j{1};
            C *k = new C(1);
        }

        // Zero initialization
        {
            static C i;
        }

        // Most vexing parse, function declaration!
        {
            C myfunc();
        }

        // The syntax goes for primitive types. These serve as the basis
        // for the recursive definition..
        {
            // Default.
            int i;
            // Following the cases at:
            // https://en.cppreference.com/w/cpp/language/default_initialization
            // i is POD and is not an array: therefore nothing is done.
            //assert(i == ?);

            // Value.
            constexpr int j{};
            // Following the cases at:
            // https://en.cppreference.com/w/cpp/language/value_initialization
            // j is not a class type, and is not an array type. Therefore it is zero initialized.
            static_assert(j == 0);

            // Direct.
            constexpr int k{1};
            // Following the cases at:
            // https://en.cppreference.com/w/cpp/language/direct_initialization
            // is not array, not class, not "if T is a non-class type but the source type is a
            // class type" whatever that means, and not bool, therefore standard conversion is used
            // and the value is set.
            static_assert(k == 1);
        }
    }

    // Now, let's see which implicit initializations are done for each case recursively.
    {
        {
            struct C { int i; };
            static_assert(std::is_default_constructible<C>());
            static_assert(std::is_aggregate<C>());

            // Default
            C a;
            // Following the cases at:
            // https://en.cppreference.com/w/cpp/language/default_initialization
            // - non-POD? no
            // - array? no
            // - then: nothing is done.
            //assert(i == ?);

            // Value
            constexpr C b{};
            // Following the cases at:
            // https://en.cppreference.com/w/cpp/language/value_initialization
            // - class type with no default constructor? no
            // - default constructor that is neither user-provided nor deleted: yes
            // - zero initialize
            // - has a non-trivial default constructor
            // - no
            assert(b.i == 0);

            // Aggregate
            constexpr C c{5};
            static_assert(c.i == 5);

            // Zero
            static C d;
            assert(d.i == 0);
        }

        {
            struct C {
                int i;
                constexpr C() : i(3) {};
                constexpr C(int i) : i(i) {};
            };
            static_assert(!std::is_pod<C>());

            // Default
            constexpr C a;
            // Following the cases at:
            // https://en.cppreference.com/w/cpp/language/default_initialization
            // - non-POD? yes. Therefore: call C(). which initializes i.
            static_assert(a.i == 3);

            // Value
            constexpr C b{};
            // Following the cases at:
            // https://en.cppreference.com/w/cpp/language/value_initialization
            // - class type with no default constructor? no
            // - user-provided default constructor? yes. Therefore, default initialize the object.
            //   So we fall on the above case, and the variable does get set.
            static_assert(b.i == 3);

            // Direct
            constexpr C c{5};
            // Following the cases at:
            // https://en.cppreference.com/w/cpp/language/_initialization
            // - array type? no
            // - class type? yes. Call constructor.
            static_assert(c.i == 5);
        }

        {
            struct C {
                int i;
                constexpr C(int i) : i(i) {};
            };
            static_assert(!std::is_pod<C>());
            static_assert(!std::is_default_constructible<C>());

#if 0
            // Cannot be default initialized if not default constructible.
            // error: no matching function
            constexpr C a;
#endif

#if 0
            // Cannot be value initialized if not default constructible.
            // error: no matching function
            C b{};
#endif

            // Direct initialize
            constexpr C c{5};
            static_assert(!std::is_pod<C>());
            // - class type with no default constructor? yes
        }

        //struct A { T t; A() : t() {} };
        //A a;   // t is value-initialized
        //A a{}; // t is value-initialized

        //struct A { T t; A() : t{} {} };
        //A a;   // t is TODO
        //A a{}; //

        //struct A { T t; A() {} };
        //A a;   // t is TODO
        //A a{}; // t is TODO

        //struct A { T t; A() = default };
        //A d;   // t is TOAO
        //A d{}; // t is TOAO

        //struct A { T t; A() = deleted };
        //A e;   // t is TODO
        //A e{}; // t is TODO

        //struct A { T t; A(T t) {} };
        //A f;   // t is TODO
        //A f{}; // t is TODO
    }
}
