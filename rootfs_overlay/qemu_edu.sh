#!/bin/sh

set -ex

# Our modules does not the PCI device yet.
lspci -k
# => 00:04.0 Class 00ff: 1234:11e8 lkmc_pci

# Interrupt counts before we generate our interrupts.
cat /proc/interrupts

# Setup.
insmod pci.ko
/mknoddev.sh lkmc_pci

# Shows that this module owns the PCI device.
lspci -k
# => 00:04.0 Class 00ff: 1234:11e8 lkmc_pci

# Identifiction: just returns some fixed magic bytes.
dd bs=4 status=none if=/dev/lkmc_pci count=1 skip=0 | od -An -t x1
# => 010000ed

# Negator. Sanity check that the hardware is getting updated.
dd bs=4 status=none if=/dev/lkmc_pci count=1 skip=1 | od -An -t x1
printf '\xF0\xF0\xF0\xF0' | dd bs=4 status=none of=/dev/lkmc_pci count=1 seek=1
dd bs=4 status=none if=/dev/lkmc_pci count=1 skip=1 | od -An -t x1
# => 0F0F0F0F

# Factorial calculator.
# Request interrupt when the computation is over.
printf '\x80\x00\x00\x00' | dd bs=4 status=none of=/dev/lkmc_pci count=1 seek=8
# factorial(0xC) = 0x1c8cfc00
printf '\x0C\x00\x00\x00' | dd bs=4 status=none of=/dev/lkmc_pci count=1 seek=2
# => irq_handler .*
# Yes, we should use the interrupt to notify poll, but lazy.
sleep 1
dd bs=4 status=none if=/dev/lkmc_pci count=1 skip=2 | od -An -t x1
dd bs=4 status=none if=/dev/lkmc_pci count=1 skip=8 | od -An -t x1
# => 1c8cfc00

# Manual IRQ raising.
printf '\x04\x03\x02\x01' | dd bs=4 status=none of=/dev/lkmc_pci count=1 seek=24
# => irq_handler .*
sleep 1
printf '\x08\x07\x06\x05' | dd bs=4 status=none of=/dev/lkmc_pci count=1 seek=24
# => irq_handler .*
sleep 1

# Teardown.
rm /dev/lkmc_pci
rmmod pci

# Interrupt counts after we generate our interrupts.
# Compare with before.
cat /proc/interrupts
