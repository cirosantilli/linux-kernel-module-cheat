#!/bin/sh
for test in \
  /anonymous_inode.sh \
  /character_device.sh \
  /character_device_create.sh \
  /debugfs.sh \
  /fops.sh \
  /procfs.sh \
  /seq_file.sh \
  /seq_file_single_open.sh \
  /sysfs.sh \
; do
  if ! "$test"; then
    echo "lkmc_test_fail: ${test}"
    exit 1
  fi
done
echo lkmc_test_pass
