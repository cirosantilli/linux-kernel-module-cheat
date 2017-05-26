#!/bin/sh

set -x
insmod /fops.ko
cd /sys/kernel/debug/lkmc_fops

## Basic read.
cat f
# => abcd
# dmesg => open
# dmesg => read
# dmesg => len = [0-9]+
# dmesg => close

## Basic write

printf '01' >f
# dmesg => open
# dmesg => write
# dmesg => len = 1
# dmesg => buf = a
# dmesg => close

cat f
# => 01cd
# dmesg => open
# dmesg => read
# dmesg => len = [0-9]+
# dmesg => close

## ENOSPC
printf '1234' >f
printf '12345' >f
echo "$?"
# => 8
cat f
# => 1234

## seek
printf '1234' >f
printf 'z' | dd bs=1 of=f seek=2
cat f
# => 12z4
