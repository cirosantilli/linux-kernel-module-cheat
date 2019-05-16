#!/bin/sh
# https://github.com/cirosantilli/linux-kernel-module-cheat#ioctl
set -e
f=/sys/kernel/debug/lkmc_ioctl
insmod ioctl.ko
[ "$(./kernel_modules/ioctl.out "$f" 0 1)" = 2 ]
[ "$(./kernel_modules/ioctl.out "$f" 1 1 1)" = '2 0' ]
rmmod ioctl
