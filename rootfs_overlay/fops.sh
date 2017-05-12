#!/bin/sh
set -ex
insmod /fops.ko
mkdir -p /fops
mount -t debugfs none /fops
cd /fops/kernel_module_cheat

cat fops
# => open
# => read
# => len = [0-9]+
# => close

printf a >fops
# => open
# => write
# => len = a
# => len = 1
# => close
