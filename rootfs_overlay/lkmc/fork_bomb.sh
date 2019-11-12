#!/bin/sh
# https://cirosantilli.com/linux-kernel-module-cheat#fork-bomb
if [ "$1" = danger ]; then
  bomb() {
    bomb | bomb &
  }
  bomb
fi
