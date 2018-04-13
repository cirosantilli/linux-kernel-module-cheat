#!/bin/sh
set -ex
d=/debugfs
mkdir -p "$d"
# We also added a fstab entry that mounts this under /sys/kernel/debug autmoatically.
# That is the most common place to mount it.
# The /sys/kernel/debug directory gets created automatically when debugfs is
# compiled into the kernel, but it does not get mounted automatically.
mount -t debugfs none /debugfs
insmod /debugfs.ko
cd "${d}/lkmc_debugfs"

cat myfile
# => 42

echo 13 > myfile
cat myfile
# => 13

echo 666 > "${d}/lkmc_debugfs_file"
cat myfile
# => 666
