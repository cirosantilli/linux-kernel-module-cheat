#!/bin/sh
# Failed attempt at debugging /init, because:
#     init must be run as pid 1
# Is this just a random BusyBox sanity check?
# - https://stackoverflow.com/questions/35019995/strace-init-process-pid-1-in-linux
/sbin/init "$@"
