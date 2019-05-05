#!/bin/sh
set -e
insmod character_device_create.ko
dev='/dev/lkmc_character_device_create_dev'
[ "$(cat "$dev")" = abcd ]
rmmod character_device_create
[ ! -e "$dev" ]
