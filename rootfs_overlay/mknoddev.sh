#!/bin/sh
dev="$1"
major="$(grep "$dev" /proc/devices | cut -d ' ' -f 1)"
mknod "/dev/$dev" c "$major" 0
