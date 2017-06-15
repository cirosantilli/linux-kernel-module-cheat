#!/bin/sh
insmod /character_device.ko
major="$(grep lkmc_character_device /proc/devices | cut -d ' ' -f 1)"
mknod /character_device.dev c $major 0
cat /character_device.dev
# => abcd
rmmod character_device
