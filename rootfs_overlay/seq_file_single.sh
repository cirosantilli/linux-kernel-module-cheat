#!/bin/sh
set -ex
insmod /seq_file_single.ko
cd /sys/kernel/debug
cat 'lkmc_seq_file_single'
# => ab
# => cd
dd if='lkmc_seq_file_single' bs=1 count=3 skip=1
# => b
# => c

rmmod seq_file_single
