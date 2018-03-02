#!/usr/bin/env bash
wget http://parsec.cs.princeton.edu/download/3.0/parsec-3.0.tar.gz
tar -xvzf parsec-3.0.tar.gz
cd parsec-3.0
. env.sh
parsecmgmt -a build -p streamcluster
parsecmgmt -a run -p streamcluster

parsecmgmt -a build -p all

wget http://parsec.cs.princeton.edu/download/3.0/parsec-3.0-input-sim.tar.gz
tar -xzf parsec-3.0-input-sim.tar.gz
parsecmgmt -a run -p streamcluster -i simsmall
parsecmgmt -a run -p streamcluster -i simlarge -n 2
