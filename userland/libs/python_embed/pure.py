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
    assert my_native_object.name() == 'ab cd'

    # Named arguments.
    my_native_object = my_native_module.MyNativeClass(first='ef', last='gh', number=13)
    assert my_native_object.name() == 'ef gh'

    # Default values and set property.
    my_native_object = my_native_module.MyNativeClass()
    my_native_object.first = 'ih'
    assert my_native_object.name() == 'ih '

    # TODO see TODO in pure.c.
    #my_derived_native_object = my_native_module.MyDerivedNativeClass('ab', 'cd', 13, 'ef', 'gh', 21)
    #print(my_derived_native_object.name2())
    return 13
