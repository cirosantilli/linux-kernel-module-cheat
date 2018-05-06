#!/bin/sh
zcat /proc/config.gz | grep -Ei "${1:-}"
