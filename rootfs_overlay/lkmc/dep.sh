#!/bin/sh
set -e
f=/sys/kernel/debug/lkmc_dep
f2=/sys/kernel/debug/lkmc_dep2

insmod dep.ko
insmod dep2.ko

# Initial value.
[ "$(cat "$f")" = 0 ]

# Changhing dep2 also changes dep.
printf 1 > "$f2"
[ "$(cat "$f")" = 1 ]

# Changhing dep also changes dep2.
printf 2 > "$f"
[ "$(cat "$f2")" = 2 ]

# sysfs shows us that the module has dependants.
[ "$(cat /sys/module/dep/refcnt)" = 1 ]
[ "$(ls /sys/module/dep/holders)" = dep2 ]
rmmod dep2.ko
[ "$(cat /sys/module/dep/refcnt)" = 0 ]
[ -z "$(ls /sys/module/dep/holders)" ]

rmmod dep.ko
