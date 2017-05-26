#!/bin/sh

set -x
insmod /fops.ko
cd /sys/kernel/debug/lkmc_fops

## Basic read.
cat fops
# => abcd
# dmesg => open
# dmesg => read
# dmesg => len = [0-9]+
# dmesg => close

## Basic write

printf '01' >fops
# dmesg => open
# dmesg => write
# dmesg => len = 1
# dmesg => buf = a
# dmesg => close

cat fops
# => 01cd
# dmesg => open
# dmesg => read
# dmesg => len = [0-9]+
# dmesg => close

## ENOSPC
printf '1234' >fops
printf '12345' >fops
echo "$?"
# => 8
cat fops
# => 1234

## seek
printf '1234' >fops
printf 'z' | dd bs=1 of=fops seek=2
cat fops
# => 12z4
