#!/usr/bin/env python3

# https://cirosantilli.com/linux-kernel-module-cheat#python

class MyClass():
    def __init__(self, mylist):
        self.mylist = mylist
    def __iter__(self):
        return iter(self.mylist)

mylist = [1, 3, 2]
i = 0
for item in MyClass(mylist):
    assert item == mylist[i]
    i += 1
