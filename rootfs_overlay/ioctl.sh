#!/bin/sh
set -e
insmod /ioctl.ko
cd /sys/kernel/debug/lkmc_ioctl/
/ioctl.out f 2 1
#/ioctl.out f 1 0
rmmod ioctl
