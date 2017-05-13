#!/bin/sh
set -ex
insmod /fops.ko
mkdir -p /fops
mount -t debugfs none /fops
cd /fops/kernel_module_cheat

cat fops
# => abcd
# dmesg => open
# dmesg => read
# dmesg => len = [0-9]+
# dmesg => close

printf a >fops
# dmesg => open
# dmesg => write
# dmesg => len = 1
# dmesg => buf = a
# dmesg => close

cd /
umount /fops
rmdir /fops
rmmod fops
