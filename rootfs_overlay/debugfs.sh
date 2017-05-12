#!/bin/sh
set -ex
mkdir -p /debugfs
mount -t debugfs none /debugfs
insmod /debugfs.ko
cd /debugfs/kernel_module_cheat
cat myfile
# => 42
