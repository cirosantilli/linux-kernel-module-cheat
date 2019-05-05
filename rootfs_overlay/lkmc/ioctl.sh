#!/bin/sh
set -e
f=/sys/kernel/debug/lkmc_ioctl
insmod ioctl.ko
[ "$(/ioctl.out "$f" 0 1)" = 2 ]
[ "$(/ioctl.out "$f" 1 1 1)" = '2 0' ]
rmmod ioctl
