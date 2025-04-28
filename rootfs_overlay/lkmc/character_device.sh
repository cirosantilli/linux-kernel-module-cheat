#!/bin/sh
set -e
insmod character_device.ko
./mknoddev.sh lkmc_character_device
[ "$(cat /dev/lkmc_character_device)" = 'abcd' ]
rm /dev/lkmc_character_device
rmmod character_device
