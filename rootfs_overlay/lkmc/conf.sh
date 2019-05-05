#!/bin/sh
# https://github.com/cirosantilli/linux-kernel-module-cheat#find-the-kernel-config
zcat /proc/config.gz | grep -Ei "${1:-}"
