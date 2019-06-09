#!/bin/sh
# https://github.com/cirosantilli/linux-kernel-module-cheat#gem5-restore-new-script
m5 checkpoint
m5 resetstats
m5 readfile | sh
m5 exit
