#!/bin/sh
# https://cirosantilli.com/linux-kernel-module-cheat#poll
set -e
insmod poll.ko
./kernel_modules/poll.out /sys/kernel/debug/lkmc_poll
#rmmod poll
