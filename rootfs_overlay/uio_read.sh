#!/bin/sh
set -ex
modprobe uio_pci_generic
# pci_min device
echo '1234 11e9' > /sys/bus/pci/drivers/uio_pci_generic/new_id
/uio_read.out &
# Helper to observe interrupts.
insmod /irq.ko
base="$(setpci -d 1234:11e9 BASE_ADDRESS_0)"
# Start generating interrupt.
devmem "0x${base}" w 0x12345678
# Stop generating interrupt.
devmem "0x$(($base + 4))" w 0x12345678
devmem "0x${base}" w 0x12345678
devmem "0x$(($base + 4))" w 0x12345678
