#!/bin/sh

set -ex

# Setup.
insmod /pci.ko
/mknoddev.sh pci

# Identifiction: just returns some fixed magic bytes.
dd bs=4 status=none if=/dev/lkmc_pci count=1 skip=0 | od -An -t x1
# => 010000ed

# Negator. Sanity check that the hardware is getting updated.
dd bs=4 status=none if=/dev/lkmc_pci count=1 skip=1 | od -An -t x1
printf '\xF0\xF0\xF0\xF0' | dd bs=4 status=none of=/dev/lkmc_pci count=1 seek=1
dd bs=4 status=none if=/dev/lkmc_pci count=1 skip=1 | od -An -t x1
# => 0F0F0F0F

# Factorial calculator.
# factorial(0xC) = 0x1c8cfc00
printf '\x0C\x00\x00\x00' | dd bs=4 status=none of=/dev/lkmc_pci count=1 seek=2
sleep 1
dd bs=4 status=none if=/dev/lkmc_pci count=1 skip=2 | od -An -t x1
dd bs=4 status=none if=/dev/lkmc_pci count=1 skip=8 | od -An -t x1
# => 1c8cfc00

# Manual IRQ raising.
printf '\x04\x03\x02\x01' | dd bs=4 status=none of=/dev/lkmc_pci count=1 seek=24
# => interrupt
sleep 1
printf '\x08\x07\x06\x05' | dd bs=4 status=none of=/dev/lkmc_pci count=1 seek=24
# => interrupt
sleep 1

# Teardown.
rm /dev/lkmc_pci
rmmod pci
