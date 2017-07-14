#!/bin/sh
set -ex
insmod /virt_to_phys.ko
cat /sys/kernel/debug/lkmc_virt_to_phys
addr=$(grep virt_to_phys /sys/kernel/debug/lkmc_virt_to_phys | cut -d ' ' -f 2)
devmem2 "$addr"
devmem2 "$addr" w 0x87654321
rmmod virt_to_phys
