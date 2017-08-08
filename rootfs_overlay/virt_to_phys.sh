#!/bin/sh
set -ex
insmod /virt_to_phys.ko
cd /sys/kernel/debug
cat lkmc_virt_to_phys
# k = 0x12345678
# i = 0x12345678
addr=$(awk '$1 == "virt_to_phys_k" { print $2 }' lkmc_virt_to_phys)
devmem "$addr"
devmem "$addr" w 0x9ABCDEF0
addr=$(awk '$1 == "virt_to_phys_i" { print $2 }' lkmc_virt_to_phys)
devmem "$addr"
devmem "$addr" w 0x9ABCDEF0
cat lkmc_virt_to_phys
# k = 0x9ABCDEF0
# i = 0x12345678
rmmod virt_to_phys
