#!/bin/sh
set -ex
insmod /fops.ko
mkdir -p /fops
mount -t debugfs none /fops
cd /fops/kernel_module_cheat
cat fops
# => open
# => read
# => close
# TODO.
#echo a >fops
