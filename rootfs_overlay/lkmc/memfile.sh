#!/bin/sh
set -eux

# Helpers
odraw() (
  od -A n -t x1 -v "$@" | tr -d '\n' | cut -c 2-
)

# Setup
f=/sys/kernel/debug/lkmc_memfile
mod="${1:-memfile.ko}"
[ $# -gt 1 ] && shift
insmod "$mod" "$@"

# Starts off empty
[ -z "$(cat "$f")" ]

# write and check it is there
printf 12 > "$f"
[ 12 = "$(cat "$f")" ]

# Append and check that it is there
printf 34 >> "$f"
[ 1234 = "$(cat "$f")" ]

# Restart
printf 56 > "$f"
[ 56 = "$(cat "$f")" ]

# skip
printf 1234 > "$f"
[ 23 = "$(dd if="$f" bs=1 count=2 skip=1)" ]

# seek
printf 1234 > "$f"
printf xy | dd bs=1 of="$f" seek=1 conv=notrunc
[ 1xy4 = "$(cat "$f")" ]

# seek past the end
printf 1234 > "$f"
printf xy | dd bs=1 of="$f" seek=6 conv=notrunc
[ '31 32 33 34 00 00 78 79' = "$(odraw "$f")" ]

# Allocate 1 GB for fun.
dd if=/dev/zero of="$f" bs=1k count=1M
[ '00 00' = "$(dd if="$f" bs=1 count=2 skip=500M | odraw)" ]

# Teardown
rmmod memfile

echo passed
