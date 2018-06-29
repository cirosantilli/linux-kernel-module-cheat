#!/bin/sh
(
  set -ex
  /character_device.sh
  /character_device_create.sh
  /fops.sh
)
if [ "$?" -eq 0 ]; then
  echo lkmc_test_pass
  exit 0
else
  echo lkmc_test_fail
  exit 1
fi
