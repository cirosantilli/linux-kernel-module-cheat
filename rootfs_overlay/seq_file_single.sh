#!/bin/sh
set -ex
insmod /seq_file_single.ko
cd /sys/kernel/debug
cat 'lkmc_seq_file_single'
# => abcd
dd if='lkmc_seq_file_single' bs=1 skip=2
# => cd
