#!/bin/sh
set -e
insmod sysfs.ko
f=/sys/kernel/lkmc_sysfs/foo
# write
printf 12345 > "$f"
# read
[ "$(cat "$f")" = 1234 ]
# seek
[ "$(dd if="$f" bs=1 count=2 skip=1 status=none)" = 23 ]
rmmod sysfs
