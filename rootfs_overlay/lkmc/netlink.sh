#!/bin/sh
# https://cirosantilli.com/linux-kernel-module-cheat#netlink-sockets
set -e
insmod netlink.ko
[ "$(./linux/netlink.out)" = 0 ]
[ "$(./linux/netlink.out)" = 1 ]
[ "$(./linux/netlink.out)" = 2 ]
rmmod netlink
