#!/bin/sh
# To be able to do init=/gem5_exit.sh, since kernel CLI argument passing is too messy:
# https://cirosantilli.com/linux-kernel-module-cheat#init-arguments
m5 exit
