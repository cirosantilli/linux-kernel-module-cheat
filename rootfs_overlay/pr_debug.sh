#!/bin/sh
echo 8 > /proc/sys/kernel/printk
echo 'file kernel/module.c +p' > /sys/kernel/debug/dynamic_debug/control
