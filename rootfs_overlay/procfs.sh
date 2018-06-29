#!/bin/sh
set -e
insmod /procfs.ko
[ "$(cat "/proc/lkmc_procfs")" = abcd ]
rmmod procfs
