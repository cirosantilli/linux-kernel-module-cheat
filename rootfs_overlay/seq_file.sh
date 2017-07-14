#!/bin/sh

set -ex

insmod /seq_file.ko
cd /sys/kernel/debug

cat 'lkmc_seq_file'
# => 0
# => 1
# => 2

cat 'lkmc_seq_file'
# => 0
# => 1
# => 2

dd if='lkmc_seq_file' bs=1 count=2 skip=0 status=none
# => 0
dd if='lkmc_seq_file' bs=1 count=4 skip=0 status=none
# => 0
# => 1
dd if='lkmc_seq_file' bs=1 count=2 skip=2 status=none
# => 1
dd if='lkmc_seq_file' bs=4 count=1 skip=0 status=none
# => 0
# => 1
