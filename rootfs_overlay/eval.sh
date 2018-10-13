#!/bin/sh
echo "$lkmc_eval"
eval "$lkmc_eval"

# Ideally, this script would do just:
#
## Get rid of the '-'.
#shift
#echo "$@"
#
# However, the kernel CLI parsing is crap, and the 4.14 docs lie.
#
# In particular, not all that is passed after "-" goes to an argument to init,
# e.g. stuff with dots like "- ./poweroff.out" still gets treated specially and
# does not go to init.
#
# This also likely means that the above solution is also unreliable in some cases,
# and that in the end you just have to add a script to the root filesystem.
