#!/bin/sh
set -e
f=/sys/kernel/debug/lkmc_kstrto
insmod kstrto.ko
printf 123 > "$f"
[ "$(cat "$f")" = 124 ]
echo foobar > "$f" && exit 1
rmmod kstrto
