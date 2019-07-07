#!/bin/sh
# https://cirosantilli.com/linux-kernel-module-cheat#replace-init
cd "$lkmc_home"
eval "$(printf "$lkmc_eval" | base64 -d)"
