#!/bin/sh
set -e
insmod mmap.ko
./kernel_modules/mmap.out /proc/lkmc_mmap 2>&1 1>/dev/null
rmmod mmap.ko
