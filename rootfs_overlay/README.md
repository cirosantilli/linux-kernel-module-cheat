# rootfs_overlay

This directory copied into the target filesystem.

We use it to for things like:

- customized configuration files
- userland module test scripts

Most tests correspond clearly to a given kernel module, but the following ones don't, e.g. they correspond to mainline tree features:

- [gpio](gpio.sh)
