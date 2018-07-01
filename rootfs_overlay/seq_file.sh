#!/bin/sh
set -e
f=/sys/kernel/debug/lkmc_seq_file
insmod /seq_file.ko
[ "$(cat "$f")" = "$(printf '0\n1\n2\n')" ]
[ "$(cat "$f")" = "$(printf '0\n1\n2\n')" ]
[ "$(dd if="$f" bs=1 count=2 skip=0 status=none)" = "$(printf '0\n')" ]
[ "$(dd if="$f" bs=1 count=2 skip=2 status=none)" = "$(printf '1\n')" ]
[ "$(dd if="$f" bs=4 count=1 skip=0 status=none)" = "$(printf '0\n1\n')" ]
rmmod seq_file
