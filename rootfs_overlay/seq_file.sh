#!/bin/sh

set -ex

insmod /seq_file.ko
cd /sys/kernel/debug

cat 'lkmc_seq_file'
# => 0
# => 1
# => 2

cat 'lkmc_seq_file'
# => 3
# => 4
# => 5

# TODO understand, why does this print nothing?
dd if='lkmc_seq_file' bs=1 count=2 skip=2
