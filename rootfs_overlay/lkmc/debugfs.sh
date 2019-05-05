#!/bin/sh
set -e
d=/debugfs
mkdir -p "$d"
mount -t debugfs none "$d"
insmod debugfs.ko
[ "$(cat "${d}/lkmc_debugfs/myfile")" = 42 ]
echo 13 > "${d}/lkmc_debugfs/myfile"
[ "$(cat "${d}/lkmc_debugfs/myfile")" = 13 ]
echo 666 > "${d}/lkmc_debugfs_file"
[ "$(cat "${d}/lkmc_debugfs/myfile")" = 666 ]
rmmod debugfs
umount "$d"
