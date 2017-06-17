#!/bin/sh
insmod /character_device.ko
/mknoddev.sh character_device
cat /dev/lkmc_character_device
# => abcd
rm /dev/lkmc_character_device
rmmod character_device
