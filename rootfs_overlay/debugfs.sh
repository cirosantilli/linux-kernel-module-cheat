#!/bin/sh
set -ex
mkdir -p /debugfs
# We also added a fstab entry that mounts this under /sys/kernel/debug autmoatically.
# That is the most common place to mount it.
# The /sys/kernel/debug directory gets created automatically when debugfs is
# compiled into the kernel, but it does not get mounted automatically.
mount -t debugfs none /debugfs
insmod /debugfs.ko
cd /debugfs/lkmc_debugfs
cat myfile
# => 42
