#!/usr/bin/env bash

# We can almost do everything from the Makefile itself by using default values for
#
#    LINUX_DIR ?= "/lib/modules/$(uname -r)/build"
#    BR2_EXTERNAL_KERNEL_MODULE_PATH="$(pwd)"
#
# The problem with that is that if you define those variables in your environment,
# the build breaks, so this is more portable.
#
# Trying to add `-i` to overcome incompatible modules will fail,
# because any build failure prevents the generation of all `.mod.c` files.

make -j $(($(nproc) - 2)) BR2_EXTERNAL_KERNEL_MODULE_PATH="$(pwd)" LINUX_DIR="/lib/modules/$(uname -r)/build" "$@"
make -C user/ -j $(($(nproc) - 2)) "$@"
