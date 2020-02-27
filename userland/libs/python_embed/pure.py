#!/usr/bin/env python3

# This is provided by the C file.
# But since we are embedding Python, we don't even need to create a separate
# .so file: it is provided directly through the Python invocation!

import my_native_module

def test_native_method(a, b):
    return my_native_module.my_native_method(a, b)

def test_native_class():
    # Positional arguments.
    my_native_object = my_native_module.MyNativeClass('ab', 'cd', 13)
    assert my_native_object.first == 'ab'
    assert my_native_object.last == 'cd'
    assert my_native_object.number == 13
    assert my_native_object.name() == 'ab cd 14'

    # Named arguments.
    my_native_object = my_native_module.MyNativeClass(first='ef', last='gh', number=13)
    assert my_native_object.name() == 'ef gh 14'

    # Default values and set property.
    my_native_object = my_native_module.MyNativeClass()
    my_native_object.first = 'ih'
    assert my_native_object.name() == 'ih  1'

    my_derived_native_object = my_native_module.MyDerivedNativeClass('ab', 'cd', 13, 'ef', 'gh', 21)
    assert my_derived_native_object.name() == 'ab cd 14'
    assert my_derived_native_object.name2() == 'ab cd 14 ef gh 23'

    return 13
