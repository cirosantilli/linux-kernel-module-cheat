#!/bin/sh
set -e
insmod /ioctl.ko
cd /sys/kernel/debug/lkmc_ioctl/
/ioctl.out f
rmmod ioctl
