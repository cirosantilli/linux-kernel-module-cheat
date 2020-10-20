// https://cirosantilli.com/linux-kernel-module-cheat#cpp-templates

#include <cassert>

struct HasMyfunc {
    int myfunc() const { return 1; }
};
struct NoHasMyfunc {
    int myfunc2() const { return 11; }
};

// int() is the actual return type.
// t.myfunc() checks that t has method myfunc().
// The decltype does not have multiple arguments, just a single comma separated expression:
// https://stackoverflow.com/questions/16044514/what-is-decltype-with-two-arguments
// Syntax breakdown:
// https://medium.com/@mortificador/choose-between-different-implementations-depending-on-type-properties-at-compile-time-in-c-68e3fd5cd2f8
template<class T>
auto template_func(const T& t) ->
    decltype(t.myfunc(), int())
{
    return t.myfunc();
}

template<class T>
auto template_func(const T& t) ->
    decltype(t.myfunc2(), int())
{
    return t.myfunc2();
}

int main() {
    // Hello world example.
    // The correct template is used for the type that has each different method.
    // If we didn't specify constraints for each template type, C++ would blow up
    // with multiple possible functions:
    // https://stackoverflow.com/questions/53441832/sfinae-for-function-body
    assert(template_func(HasMyfunc()) == 1);
    assert(template_func(NoHasMyfunc()) == 11);
}
