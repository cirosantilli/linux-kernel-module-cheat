#!/bin/sh

set -x
insmod /sysfs.ko
cd /sys/kernel/lkmc_sysfs
printf 12345 >foo
cat foo
# => 1234
dd if=foo bs=1 count=2 skip=1 status=none
# => 23
rmmod sysfs
