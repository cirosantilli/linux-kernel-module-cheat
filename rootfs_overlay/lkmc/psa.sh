#!/bin/sh
# Show All possible ps fields to get better process visibility.
# TODO for some reason nothing shows after args, so we put it last.
ps -o user,group,comm,pid,ppid,sid,pgid,tty,vsz,rss,stat,args
