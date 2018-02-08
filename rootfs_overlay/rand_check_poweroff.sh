#!/bin/sh
set -ex
/rand_check.out

# Check if network is being replayed.
#
# TODO; requires internet to be up, which is done by inittab.
# We could do this with a /etc/init.d/SXX file,
# but how to both check that into git, and make it optional?
#
# https://superuser.com/questions/635020/how-to-know-current-time-from-internet-from-command-line-in-linux
# Raw IP because was failing with hostname:
# https://unix.stackexchange.com/questions/124283/busybox-ping-ip-works-but-hostname-nslookup-fails-with-bad-address
# https://tf.nist.gov/tf-cgi/servers.cgi
#
#echo | nc 129.6.15.28 13

# busybox's poweroff panics, TODO why. Likely tries to kill shell.
# So just use our super raw command.
/poweroff.out
