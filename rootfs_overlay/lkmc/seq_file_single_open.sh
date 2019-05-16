#!/bin/sh
set -e
f=/sys/kernel/debug/lkmc_seq_file_single_open
insmod seq_file_single_open.ko
[ "$(cat "$f")" = "$(printf 'ab\ncd\n')" ]
[ "$(dd if="$f" bs=1 count=3 skip=1)" = "$(printf "b\nc\n")" ]
rmmod seq_file_single_open
