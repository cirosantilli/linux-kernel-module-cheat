// https://cirosantilli.com/linux-kernel-module-cheat#cpp

#include <cassert>

int main() {

    // Hello world.
    {
        struct Base {
            virtual int f() { return 0; }
        };

        struct Derived1 : public Base {
            virtual int f() override { return 1; }
        };

        struct Derived2 : public Base {
            // virtual not required on the last derived method.
            int f() override { return 2; }
        };

        // Depending on what bp actually points to, we call different functions.
        // This is what is called polymorphism.

        Base *bp;

        Base b;
        bp = &b;
        assert(bp->f() == 0);

        Derived1 d1;
        bp = &d1;
        assert(bp->f() == 1);

        Derived2 d2;
        bp = &d2;
        assert(bp->f() == 2);
    }

    // Without `virtual`, no polymorphism happens!
    // We get name hiding instead.
    {
        struct Base {
            int f() { return 0; }
        };

        struct Derived : public Base {
            int f() { return 1; }
        };

        Base *bp;

        Base b;
        bp = &b;
        assert(bp->f() == 0);

        Derived d;
        bp = &d;
        // Base method called!
        assert(bp->f() == 0);
    }

    // If the derived signature is not compatible with the virtual,
    // no polymorphism happens!
    //
    // Also see "covariant return" for compatibility of return types.
    //
    // This is why you should always use override to prevent such bugs.
    {
        struct Base {
            virtual int f() { return 0; }
        };

        class Derived : public Base {
            virtual int f(int i) /* override */ { return i; }
        };

        Base *bp;

        Base b;
        bp = &b;
        assert(bp->f() == 0);

        Derived d;
        bp = &d;
        // Base method called!
        assert(bp->f() == 0);

        // ERROR: no matching function.
#if 0
        assert(bp->f(1) == 1);
#endif
    }

    // Polymorphic calls cannot be made from constructors.
    // https://stackoverflow.com/questions/1453131/how-can-i-get-polymorphic-behavior-in-a-c-constructor
    {
        struct Base {
            int i;
            Base() {
                i = f();
            }
            virtual int f() { return 0; }
        };

        class Derived : public Base {
            int f() override { return i; }
        };

        Base b;
        assert(b.i == 0);

        Derived d;
        assert(d.i == 0);
    }
}
