#!/bin/sh
zcat /proc/config.gz | grep "${1:-}"
