#!/bin/sh
set -ex
/rand_check.out

# Check if network is being replayed.
# https://superuser.com/questions/635020/how-to-know-current-time-from-internet-from-command-line-in-linux
# https://tf.nist.gov/tf-cgi/servers.cgi
#echo | nc 129.6.15.28 13

# busybox's poweroff panics, TODO why. Likely tries to kill shell.
# So just use our super raw command.
/poweroff.out
