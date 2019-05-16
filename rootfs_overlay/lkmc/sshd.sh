#!/bin/sh
/sbin/ifup -a
/usr/bin/ssh-keygen -A
/usr/sbin/sshd
