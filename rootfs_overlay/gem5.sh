#!/bin/sh
m5 checkpoint
script=/tmp/readfile
m5 readfile > "$script"
if [ -s "$script" ]; then
  sh "$script"
fi
