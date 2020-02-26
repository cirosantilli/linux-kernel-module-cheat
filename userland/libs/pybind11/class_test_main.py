#!/usr/bin/env python3

import class_test

my_class_test = class_test.ClassTest('abc');
assert(my_class_test.getName() == 'abc')
my_class_test.setName('012')
assert(my_class_test.name == '012')

my_class_test_derived = class_test.ClassTestDerived('abc', 'def');
assert(my_class_test_derived.getName2() == 'abcdef2')
