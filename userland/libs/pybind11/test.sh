#!/usr/bin/env bash
set -eux
g++ `python3-config --cflags` -shared -std=c++11 -fPIC class_test.cpp -o class_test`python3-config --extension-suffix` `python3-config --libs`
./class_test_main.py
