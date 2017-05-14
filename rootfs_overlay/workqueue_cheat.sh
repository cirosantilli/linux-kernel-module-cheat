#!/bin/sh

# Got a few kernel crashes after insert / remove, so let's do this a few times.
insmod /workqueue_cheat.ko
rmmod workqueue_cheat
insmod /workqueue_cheat.ko
rmmod workqueue_cheat
insmod /workqueue_cheat.ko
rmmod workqueue_cheat
