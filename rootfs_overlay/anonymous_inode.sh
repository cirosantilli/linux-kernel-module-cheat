#!/bin/sh
set -e
insmod /anonymous_inode.ko
[ "$(/anonymous_inode.out /sys/kernel/debug/lkmc_anonymous_inode 3)" = "$(printf '1\n10\n100')" ]
rmmod anonymous_inode
