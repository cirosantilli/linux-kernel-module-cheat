#!/bin/sh
set -e
insmod /anonymous_inode.ko
cd /sys/kernel/debug/lkmc_anonymous_inode/
/anonymous_inode.out f
rmmod anonymous_inode
