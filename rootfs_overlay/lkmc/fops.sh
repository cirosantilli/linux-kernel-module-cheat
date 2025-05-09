#!/bin/sh
set -e

# Setup
f=/sys/kernel/debug/lkmc_fops
mod="${1:-fops.ko}"
[ $# -gt 1 ] && shift
insmod "$mod" "$@"

# read
[ "$(cat "$f")" = abcd ]

# write
printf 01 > "$f"
[ "$(cat "$f")" = 01cd ]

# ENOSPC
printf abcd > "$f"
set +e
printf 12345 > "$f"
exit_status="$?"
set -e
[ "$exit_status" -eq 1 ]
[ "$(cat "$f")" = abcd ]

# seek
printf 1234 > "$f"
printf z | dd bs=1 of="$f" seek=2
[ "$(cat "$f")" = 12z4 ]

# seek past the end
printf 1234 > "$f"
printf xy | dd bs=1 of="$f" seek=6
[ "$(cat "$f")" = 1234 ]

# Teardown
rmmod fops

echo passed
