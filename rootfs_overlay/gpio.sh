#!/bin/sh
set -e
cd /sys/class/gpio
echo 480 > export
echo 481 > export
echo 482 > export
echo 488 > export
echo 496 > export
echo out > gpio480/direction
echo out > gpio481/direction
echo out > gpio482/direction
echo out > gpio488/direction
echo out > gpio496/direction
v=1
while true; do
  echo $v > gpio480/value
  echo $v > gpio481/value
  echo $v > gpio482/value
  echo $v > gpio488/value
  echo $v > gpio496/value
  if [ $v -eq 1 ]; then
    v=0
  else
    v=1
  fi
  sleep 1
done
