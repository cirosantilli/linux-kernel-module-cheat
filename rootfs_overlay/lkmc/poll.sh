#!/bin/sh
set -e
insmod poll.ko
./poll.out /sys/kernel/debug/lkmc_poll
#rmmod poll
