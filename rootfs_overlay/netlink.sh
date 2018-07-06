#!/bin/sh
set -e
insmod /netlink.ko
[ "$(/netlink.out)" = 0 ]
[ "$(/netlink.out)" = 1 ]
[ "$(/netlink.out)" = 2 ]
rmmod netlink
