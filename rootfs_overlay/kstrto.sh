#!/bin/sh
insmod /kstrto.ko
cd /sys/kernel/debug
echo 1234 > lkmc_kstrto
# 1234
echo foobar > lkmc_kstrto
# -22
