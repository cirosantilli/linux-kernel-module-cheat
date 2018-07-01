#!/bin/sh
set -e
d=/sys/module/params/parameters
i="${d}/i"
j="${d}/j"
insmod /params.ko
# dmesg => 0 0
[ "$(cat "$i")" = '1 0' ]
printf 1 > "$i"
# dmesg => 1 0
rmmod params

insmod /params.ko i=1 j=1
# dmesg => 1 1
rmmod params
