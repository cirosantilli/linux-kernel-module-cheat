# Linux Kernel Module Cheat

Run one command, get a QEMU Buildroot BusyBox virtual machine built from source with several minimal Linux kernel 4.14 module development example tutorials with GDB and KGDB step debugging and minimal educational hardware models. "Tested" in x86, ARM and MIPS guests, Ubuntu 17.10 host.

![](screenshot.png)

Reserve 12Gb of disk and run:

    git clone https://github.com/cirosantilli/linux-kernel-module-cheat
    cd linux-kernel-module-cheat
    ./configure
    ./build
    ./run

The first build will take a while ([GCC](https://stackoverflow.com/questions/10833672/buildroot-environment-with-host-toolchain), Linux kernel), e.g.:

- 2 hours on a mid end 2012 laptop
- 30 minutes on a high end 2017 desktop

If you don't want to wait, you could also try to compile the examples and run them on your host computer as explained on the ["Run on host" section](run-on-host.md), but as explained on that section, that is dangerous, limited, and will likely not work.

After QEMU opens up, you can start playing with the kernel modules:

    root
    insmod /hello.ko
    insmod /hello2.ko
    rmmod hello
    rmmod hello2

This should print to the screen:

    hello init
    hello2 init
    hello cleanup
    hello2 cleanup

which are `printk` messages from `init` and `cleanup` methods of those modules.

All available modules can be found in the [`kernel_module` directory](kernel_module/).

See the [getting started section](getting-started.md) for further details.

1.  [**Getting started**](getting-started.md)
1.  Action
    1.  Step debugging
        1.  [GDB step debugging](gdb-step-debugging.md)
        1.  [KGDB](kgdb.md)
        1.  [gdbserver](gdbserver.md)
    1.  [Other architectures](other-architectures.md)
    1.  [modprobe](modprobe.md)
    1.  [X11](x11.md)
    1.  [Count boot instructions](count-boot-instructions.md)
    1.  [ftrace](ftrace.md)
    1.  [Device tree](device-tree.md)
1.  Failed action
    1.  [Record and replay](record-and-replay.md)
    1.  [GEM5](gem5.md)
1.  Insane action
    1.  [Run on host](run-on-host.md)
    1.  [Hello host](hello_host/)
1.  Conversation
    1.  [kmod](kmod.md)
    1.  [Maintainers](maintainers.md)
    1.  [Bibliography](bibliography.md)
