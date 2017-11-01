# Getting started

## Insane unsafe host super fast quickstart

    cd kernel_module
    ./make-host.sh

If the compilation of any of the C files fails, e.g. because of kernel or toolchain differences that we don't control on the host, just rename it to remove the `.c` extension and try again:

    mv broken.c broken.c~
    ./build_host

Once you manage to compile, try it out with:

    sudo insmod hello.ko

    # Our module is there.
    sudo lsmod | grep hello

    # Last message should be: hello init
    dmest -T

    sudo rmmod hello

    # Last message should be: hello exit
    dmesg -T

    # Not present anymore
    sudo lsmod | grep hello

Why this is very bad and you should be ashamed:

-   bugs can easily break you system. E.g.:
    - segfaults can trivially lead to a kernel crash, and require a reboot
    - your disk could get erased. Yes, this can also happen with `sudo` from userland. But you should not use `sudo` when developing newbie programs. And for the kernel you don't have the choice not to use `sudo`
    - even more subtle system corruption such as [not being able to rmmod](https://unix.stackexchange.com/questions/78858/cannot-remove-or-reinsert-kernel-module-after-error-while-inserting-it-without-r)
-   can't control which kernel version and build options to use. So some of the modules may simply not compile because of kernel API changes, since [the Linux kernel does not have a stable kernel module API](https://stackoverflow.com/questions/37098482/how-to-build-a-linux-kernel-module-so-that-it-is-compatible-with-all-kernel-rele/45429681#45429681).
-   can't control which hardware is used, notably the CPU architecture
-   can't step debug it with GDB easily

The only advantage of using your host machine, is that you don't have to wait 2 hours and use up 8 Gigs for the build. But you will soon find out that this is a very reasonable price to pay.

## Do the right thing and use a virtual machine

Reserve 12Gb of disk:

    git clone --recursive https://github.com/cirosantilli/linux-kernel-module-cheat
    cd linux-kernel-module-cheat
    sudo apt-get install libsdl2-dev
    sudo apt-get build-dep qemu
    ./run

The first build will take a while ([GCC](https://stackoverflow.com/questions/10833672/buildroot-environment-with-host-toolchain), Linux kernel), e.g.:

- 2 hours on a mid end 2012 laptop
- 30 minutes on a high end 2017 desktop

QEMU opens up, and you can run:

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

Each module comes from a C file under `kernel_module/`. For module usage see:

    head kernel_module/modulename.c

Good bets inside guest are:

    /modulename.sh
    /modulename.out

## Save rebuild time

After the first build, you can also run just:

    ./runqemu

to save a few seconds. `./run` wouldn't rebuild everything, but checking timestamps takes a few moments.

If you make changes to the kernel modules or most configurations, you can just use again:

    ./run

and they will updated.

But if you change any package besides `kernel_module`, you must also request those packages to be reconfigured or rebuilt with extra targets, e.g.:

    ./run -t linux-reconfigure -t host-qemu-reconfigure

Those aren't turned on by default because they take quite a few seconds.

## Filesystem persistency

The root filesystem is persistent across:

    ./runqemu
    date >f
    sync

then:

    ./runqemu
    cat f

This is particularly useful to re-run shell commands from the history of a previous session with `Ctrl + R`.

When you do:

    ./run

the disk image gets overwritten by a fresh filesystem and you lose all changes.

Remember that if you forcibly turn QEMU off without `sync` or `poweroff` from inside the VM, disk changes may not be saved.

## Message control

We use `printk` a lot, and it shows on the QEMU terminal by default. If that annoys you (e.g. you want to see stdout separately), do:

    dmesg -n 1

See also: <https://superuser.com/questions/351387/how-to-stop-kernel-messages-from-flooding-my-console>

You can scroll up a bit on the default TTY with:

    Shift + PgUp

but I never managed to increase that buffer:

- <https://askubuntu.com/questions/709697/how-to-increase-scrollback-lines-in-ubuntu14-04-2-server-edition>
- <https://unix.stackexchange.com/questions/346018/how-to-increase-the-scrollback-buffer-size-for-tty>

## Text mode

Show serial output of QEMU directly on the current terminal, without opening a QEMU window:

    ./runqemu -n

To exit, just do a regular:

    poweroff

This mode is very useful to:

- get full panic traces when you start making the kernel crash :-) See also: <https://unix.stackexchange.com/questions/208260/how-to-scroll-up-after-a-kernel-panic>
- copy and paste commands and stdout output to / from host
- have a large scroll buffer, and be able to search it, e.g. by using GNU `screen` on host

If the system crashes, you can't can quit QEMU with `poweroff`, but you can use either:

    Ctrl-C X

or:

    Ctrl-C A
    quit

or:

    ./qemumonitor
    quit

or:

    echo quit | ./qemumonitor

See also:

- <http://stackoverflow.com/questions/14165158/how-to-switch-to-qemu-monitor-console-when-running-with-curses>
- <https://superuser.com/questions/1087859/how-to-quit-qemu-monitor>
- <https://superuser.com/questions/488263/problems-switching-to-qemu-control-panel-with-nographics>
- <https://superuser.com/questions/1087859/how-to-quit-the-qemu-monitor-when-not-using-a-gui/1211516#1211516>

Limitations:

-   TODO: Ctrl + C kills the emulator for some setups (TODO which what exactly?), and not sent to guest processes. See:

    - <https://github.com/cloudius-systems/osv/issues/49>
    - <https://unix.stackexchange.com/questions/167165/how-to-pass-ctrl-c-in-qemu>

    This is however fortunate when running QEMU with GDB, as the Ctrl + C reaches GDB and breaks.

-   Very early kernel messages such as `early console in extract_kernel` only show on the GUI, since at such early stages, not even the serial has been setup.

## Automatic startup commands

When debugging a module, it becomes tedious to wait for build and re-type:

    root
    /mymod.sh

Instead, you can add your test commands to:

    cd rootfs_overlay/etc/init.d
    cp S98 S99
    vim S99

and they will be run automatically before the login prompt.

`S99` is already gitignored for you.

For convenience, we also setup a symlink from `S99` to `rootfs_overlay/etc/init.d/S99`.

Scripts under `/etc/init.d` are run by `/etc/init.d/rcS`, which gets called by the line `::sysinit:/etc/init.d/rcS` in `/etc/inittab`.

## Kernel version

We use Buildroot's default kernel version with small educational patches on top, you can confirm it after build with:

    grep BR2_LINUX_KERNEL_VERSION buildroot/output.*~/.config

or in QEMU:

    cat /proc/version

or in the source:

    cd linux
    git log | grep -E '    Linux [0-9]+\.' | head

## insmod alternatives

If you are feeling fancy, you can also insert modules with:

    modprobe hello

This method also deals with module dependencies, which we almost don't use to make examples simpler:

- <https://askubuntu.com/questions/20070/whats-the-difference-between-insmod-and-modprobe>
- <https://stackoverflow.com/questions/22891705/whats-the-difference-between-insmod-and-modprobe>

`modprobe` searches for modules under:

    ls /lib/modules/*/extra/

Kernel modules built from the Linux mainline tree with `CONFIG_SOME_MOD=m`, are automatically available with `modprobe`, e.g.:

    modprobe dummy-irq

If you are feeling raw, you can use our own minimal:

    /myinsmod.out /hello.ko

which demonstrates the C module API: <https://stackoverflow.com/questions/5947286/how-can-linux-kernel-modules-be-loaded-from-c-code/38606527#38606527>

## QEMU GUI is unresponsive

Sometimes in Ubuntu 14.04, after the QEMU SDL GUI starts, it does not get updated after keyboard strokes, and there are artifacts like disappearing text.

We have not managed to track this problem down yet, but the following workaround always works:

    Ctrl + Shift + U
    Ctrl + C
    root

This started happening when we switched to building QEMU through Buildroot, and has not been observed on later Ubuntu.

Using text mode is another workaround if you don't need GUI features.

## Debug QEMU

When you start interacting with QEMU hardware, it is useful to see what is going on inside of QEMU itself.

This is of course trivial since QEMU is just an userland program on the host, but we make it a bit easier with:

    ./runqemu -q

Then you could:

    b edu_mmio_read
    c

And in QEMU:

    /pci.sh

Just make sure that you never click inside the QEMU window when doing that, otherwise you mouse gets captured forever, and the only solution I can find is to go to a TTY with Ctrl + Alt + F1 and `kill` QEMU.

You can still send key presses to QEMU however even without the mouse capture, just either click on the title bar, or alt tab to give it focus.
