#!/bin/sh
# This covers the most common setup to run a benchmark in gem5 and exit.
m5 checkpoint
m5 resetstats
m5 readfile | sh
m5 exit
