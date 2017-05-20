#!/bin/sh
# Generate infinitely many system calls :-)
i=0
while true; do
  echo "$i"
  i=$(($i+1))
  sleep 1
done
