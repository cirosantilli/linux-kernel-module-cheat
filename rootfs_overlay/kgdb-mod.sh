#!/bin/sh
insmod /fops.ko
cd /sys/kernel/debug/kernel_module_cheat
i=0
while true; do
  printf "$i" >fops
  i=$(($i+1))
done &
/kgdb.sh
