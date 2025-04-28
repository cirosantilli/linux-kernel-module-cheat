#!/bin/sh
set -eux

name=scull

mod="${1:-$name.ko}"
shift
insmod "$mod" "$@"
major="$(awk "\$2==\"$name\" {print \$1}" /proc/devices)"

rm -f /dev/${name}[0-3]
mknod /dev/${name}0 c $major 0
mknod /dev/${name}1 c $major 1
mknod /dev/${name}2 c $major 2
mknod /dev/${name}3 c $major 3

rm -f /dev/${name}pipe[0-3]
mknod /dev/${name}pipe0 c $major 4
mknod /dev/${name}pipe1 c $major 5
mknod /dev/${name}pipe2 c $major 6
mknod /dev/${name}pipe3 c $major 7

rm -f /dev/${name}single
mknod /dev/${name}single c $major 8

rm -f /dev/${name}uid
mknod /dev/${name}uid c $major 9

rm -f /dev/${name}wuid
mknod /dev/${name}wuid c $major 10

rm -f /dev/${name}priv
mknod /dev/${name}priv c $major 11

## scull

f="/dev/${name}0"
[ -z "$(cat "$f")" ]

# Append starts writing from the start of the 4k block, not like the usual semantic.
printf asdf > "$f"
printf qw >> "$f"
[ qwdf = "$(cat "$f")" ]

# Overwrite first clears everything, then writes to start of 4k block.
printf asdf > /dev/${name}0
printf qw > /dev/${name}0
[ qw = "$(cat "$f")" ]

# Read from the middle
printf asdf > /dev/${name}0
[ df = "$(dd if="$f" bs=1 count=2 skip=2 status=none)" ]

# Write to the middle
printf asdf > "$f"
printf we | dd of="$f" bs=1 seek=1 conv=notrunc status=none
[ awef = "$(cat "$f")" ]

echo passed
