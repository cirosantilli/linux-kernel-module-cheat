#!/bin/sh
set -e
insmod /anonymous_inode.ko
/anonymous_inode.out /sys/kernel/debug/lkmc_anonymous_inode
rmmod anonymous_inode
