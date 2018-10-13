#!/bin/sh
insmod pmccntr.ko
cd /sys/kernel/debug
cat lkmc_pmccntr
