#!/bin/sh
set -ex
lower="${1:-/}"
mount="$2"
if [ -z "$mount" ]; then
  mount="$lower"
fi
upper="${lower}/upper"
work="${lower}/work"
mkdir -p "$lower" "$upper" "$work" "$mount"
touch "${lower}/asdf" "${upper}/qwer"
mount -t overlay -o lowerdir="$lower",upperdir="$upper",workdir="$work" none "$mount"
ls "$lower" "$upper"
umount "$mount"
