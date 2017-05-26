#!/bin/sh
insmod /fops.ko
cd /sys/kernel/debug/lkmc_fops
i=0
while true; do
  printf "$i" >fops
  i=$(($i+1))
done &
/kgdb.sh
