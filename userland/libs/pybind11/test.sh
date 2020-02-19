#!/usr/bin/env bash
set -eu
g++ -O3 -Wall -shared -std=c++11 -fPIC  class_test.cpp -o class_test`python3-config --extension-suffix` -I /usr/include/python3.6m
./class_test_main.py
