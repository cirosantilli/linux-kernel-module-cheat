#!/bin/sh
# https://cirosantilli.com/linux-kernel-module-cheat#find-the-kernel-config
zcat /proc/config.gz | grep -Ei "${1:-}"
