#!/bin/sh
set -ex
insmod /mmap.ko
/mmap.out /proc/lkmc_mmap
rmmod /mmap.ko
