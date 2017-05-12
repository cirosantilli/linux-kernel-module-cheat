# Linux Kernel Module Cheat

Run one command, get into QEMU Buildroot BusyBox with several minimal Linux kernel module examples. Tested in Ubuntu 14.04.

Usage:

    ./run

First build will take a while (GCC, Linux kernel).

QEMU opens up, and you can run:

    insmod /hello.ko
    insmod /hello2.ko
    rmmod hello
    rmmod hello2

Each module comes from a C file under `kernel_module/`. `head *.c` for module usage.

The Linux kernel version can be found with:

    grep BR2_LINUX_KERNEL_VERSION buildroot/.config

1.  [Introduction](introduction.md)
1.  [Build](build.md)
1.  [kmod](kmod.md)
1.  Examples
    1.  [Host](host/)
    1.  Buildroot
        1. [hello](kernel_module/hello.c)
        1. [hello2](kernel_module/hello2.c)
        1. [debugfs](kernel_module/debugfs.c)
