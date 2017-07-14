#!/bin/sh
set -e
insmod /ioctl.ko
/ioctl.out /sys/kernel/debug/lkmc_ioctl
rmmod ioctl
