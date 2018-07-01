#!/bin/sh
set -e
d=/sys/module/params/parameters
i="${d}/i"
j="${d}/j"
f=/sys/kernel/debug/lkmc_params

insmod /params.ko
[ "$(cat "$i")" = 0 ]
[ "$(cat "$j")" = 0 ]
[ "$(cat "$f")" = '0 0' ]
printf 1 > "$i"
[ "$(cat "$f")" = '1 0' ]
printf 2 > "$j"
[ "$(cat "$f")" = '1 2' ]
rmmod params

insmod /params.ko i=3 j=4
[ "$(cat "$f")" = '3 4' ]
rmmod params
