#!/bin/sh
set -e

# Setup
f=/sys/kernel/debug/lkmc_fops
insmod /fops.ko

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
[ "$exit_status" -eq 8 ]
[ "$(cat "$f")" = abcd ]

# seek
printf 1234 > "$f"
printf z | dd bs=1 of="$f" seek=2
[ "$(cat "$f")" = 12z4 ]

# Teardown
rmmod fops
