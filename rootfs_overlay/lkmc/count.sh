#!/bin/sh
# Count to infinity with 1 second sleep between each increment.
# Generate infinitely many system calls :-)
i=0
while true; do
  echo "$i"
  i=$(($i+1))
  sleep 1
done
