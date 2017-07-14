#!/bin/sh
set -ex
insmod /virt_to_phys.ko
cd /sys/kernel/debug
cat lkmc_virt_to_phys
addr=$(grep virt_to_phys lkmc_virt_to_phys | cut -d ' ' -f 2)
devmem2 "$addr"
devmem2 "$addr" w 0x9ABCDEF0
cat lkmc_virt_to_phys
rmmod virt_to_phys
