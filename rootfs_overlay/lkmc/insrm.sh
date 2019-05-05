#!/bin/sh
# Insert and remove a module n times to check for spurious errors / deadlocks.
set -e
mod="$1"
n="${2:-1}"
i=0
while [ $i -lt $n ]; do
  echo "insmod $i"
  insmod "/$mod.ko"
  echo "rmmod $i"
  rmmod "$mod"
  i=$(($i+1))
done
