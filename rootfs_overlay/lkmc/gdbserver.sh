#!/bin/sh
/sbin/ifup -a
gdbserver :45455 "$@"
