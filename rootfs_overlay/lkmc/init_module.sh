#!/bin/sh
set -e
insmod init_module.ko
rmmod init_module
