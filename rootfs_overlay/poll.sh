#!/bin/sh
insmod /poll.ko
/poll.out /sys/kernel/debug/lkmc_poll
