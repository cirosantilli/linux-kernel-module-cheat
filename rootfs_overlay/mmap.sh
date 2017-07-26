#!/bin/sh
insmod /mmap.ko
/mmap.out /sys/kernel/debug/lkmc_mmap
rmmod /mmap.ko
