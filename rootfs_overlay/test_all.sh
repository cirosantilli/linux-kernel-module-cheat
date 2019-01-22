#!/bin/sh
test_dir="${1:-.}"
for test in \
  anonymous_inode.sh \
  character_device.sh \
  character_device_create.sh \
  debugfs.sh \
  dep.sh \
  fops.sh \
  init_module.sh \
  ioctl.sh \
  kstrto.sh \
  mmap.sh \
  netlink.sh \
  params.sh \
  procfs.sh \
  seq_file.sh \
  seq_file_single_open.sh \
  sysfs.sh \
; do
  if ! "${test_dir}/${test}"; then
    echo "Test failed: ${test}"
    test_fail.sh
    exit 1
  fi
done
echo 'All tests passed.'
