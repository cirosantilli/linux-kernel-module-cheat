#!/bin/sh
cat /dev/urandom > "/dev/fb${1:-0}"
