# Linux Kernel Module Cheat

Run one command, get a QEMU Buildroot BusyBox virtual machine with several minimal Linux kernel 4.9 module development example tutorials with GDB and KGDB debug. Tested in x86, ARM and MIPS guests, Ubuntu 14.04 - 17.04 hosts.

![](screenshot.png)

## Insane unsafe host super fast quickstart

    cd kernel_module
    ./make-host.sh

If the compilation of any of the C files fails (because of kernel or toolchain differences that we don't control on the host), just rename it to remove the `.c` extension and try again:

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

    sudo apt-get build-dep qemu
    ./run

First build will take a while (GCC, Linux kernel).

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

But if you change any package besides `kernel_module`, you must also request those packages to be reconfigured or rebuilt with extra Targets, e.g.:

    ./run -t linux-reconfigure -t host-qemu-rebuild

Those aren't turned on by default because they take quite a few seconds.

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

which demonstrates the C module API.

## Message control

We use `printk` a lot, and it shows on the QEMU terminal by default. If that annoys you (e.g. you want to see stdout separately), do:

    dmesg -n 1

See also: <https://superuser.com/questions/351387/how-to-stop-kernel-messages-from-flooding-my-console>

You can scroll up a bit on the default TTY with:

    Shift + PgUp

but I never managed to increase that buffer:

- <https://askubuntu.com/questions/709697/how-to-increase-scrollback-lines-in-ubuntu14-04-2-server-edition>
- <https://unix.stackexchange.com/questions/346018/how-to-increase-the-scrollback-buffer-size-for-tty>

## Kernel version

We use Buildroot's default kernel version, you can confirm it after build with:

    grep BR2_LINUX_KERNEL_VERSION buildroot/output.*~/.config

or in QEMU:

    cat /proc/version

## QEMU GUI is unresponsive

Sometimes in Ubuntu 14.04, after the QEMU SDL GUI starts, it does not get updated after keyboard strokes, and there are artifacts like disappearing text.

We have not managed to track this problem down yet, but the following workaround always works:

    Ctrl + Shift + U
    Ctrl + C
    root

This started happening when we switched to building QEMU through Buildroot, and has not been observed on later Ubuntu.

Using text mode is another workaround if you don't need GUI features.

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

TODO: Ctrl + C kills the emulator, it is not sent to guest processes. See:

- <https://github.com/cloudius-systems/osv/issues/49>
- <https://unix.stackexchange.com/questions/167165/how-to-pass-ctrl-c-in-qemu>

This is however fortunate when running QEMU with GDB, as the Ctrl + C reaches GDB and breaks.

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

## Debugging

To GDB the Linux kernel, first run:

    ./runqemu -d

If you want to break immediately at a symbol, e.g. `start_kernel` of the boot sequence, run on another shell:

    ./rungdb start_kernel

Now QEMU will stop there, and you can use the normal GDB commands:

    l
    n
    c

To skip the boot, run just:

    ./rungdb

and when you want to break, do `Ctrl + C` from GDB.

To have some fun, you can first run inside QEMU:

    /count.sh

which counts to infinity to stdout, and then in GDB:

    Ctrl + C
    break sys_write
    continue
    continue
    continue

And you now control the counting from GDB.

See also: <http://stackoverflow.com/questions/11408041/how-to-debug-the-linux-kernel-with-gdb-and-qemu>

If you are using text mode:

    ./runqemu -d -n

QEMU cannot be put on the background of the current shell, so you will need to open a separate terminal and run:

    ./rungdb

`O=0` is however an impossible dream, `O=2` being the default: <https://stackoverflow.com/questions/29151235/how-to-de-optimize-the-linux-kernel-to-and-compile-it-with-o0> So get ready for some weird jumps, and `<value optimized out>` fun. Why, Linux, why.

### Kernel module debugging

Loadable kernel modules are a bit trickier since the kernel can place them at different memory locations depending on load other.

So we cannot set the breakpoints before `insmod`.

However, the Linux kernel GDB scripts offer the `lx-symbols` command, which takes care of that beautifully for us:

    ./runqemu -d
    ./rungdb

In QEMU:

    insmod /fops.ko

In GDB, hit `Ctrl + C`, and note how it says:

    scanning for modules in ../kernel_module-1.0/
    loading @0xffffffffa0000000: ../kernel_module-1.0//fops.ko

That's `lx-symbols` working! Now simply:

    b fop_write
    c

In QEMU:

    printf a >/sys/kernel/debug/lkmc_fops/f

and GDB now breaks at our `fop_write` function!

Just don't forget to remove your breakpoints after `rmmod`, or they will point to stale memory locations.

TODO: why does `break work_func` for `insmod kthread.ko` not break the first time I `insmod`, but breaks the second time?

#### Bypassing lx-symbols

Useless, but a good way to show how hardcore you are. From inside QEMU:

    insmod /fops.ko
    cat /proc/modules

This will give a line of form:

    fops 2327 0 - Live 0xfffffffa00000000

And then tell GDB where the module was loaded with:

    Ctrl + C
    add-symbol-file ../kernel_module-1.0/fops.ko 0xfffffffa00000000

## Other architectures

The portability of the kernel and toolchains is amazing: change an option and most things magically work on completely different hardware.

### arm

First build:

    ./run -a arm

Run without build:

    ./runqemu -a arm

Debug:

    ./runqemu -a arm -d
    # On another terminal.
    ./rungdb -a arm

TODOs:

-   only managed to run in the terminal interface (but weirdly a blank QEMU window is still opened)
-   GDB not connecting to KGDB. Possibly linked to `-serial stdio`. See also: <https://stackoverflow.com/questions/14155577/how-to-use-kgdb-on-arm>

### aarch64

    ./run -a aarch64

TODOs:

-   GDB gives a ton of messages:

        no module object found for ''

    when you connect. `Ctrl + C` then `c` works though.

-   How to add devices to `-M virt` as we did for `-M versatilepb`

### mips64

    ./run -a mips64

## KGDB

KGDB is kernel dark magic that allows you to GDB the kernel on real hardware without any extra hardware support.

It is useless with QEMU since we already have full system visibility with `-gdb`, but this is a good way to learn it.

Cheaper than JTAG (free) and easier to setup (all you need is serial), but with less visibility as it depends on the kernel working, so e.g.: dies on panic, does not see boot sequence.

Usage:

    ./runqemu -k
    ./rungdb -k

In GDB:

    c

In QEMU:

    /count.sh &
    /kgdb.sh

In GDB:

    b sys_write
    c
    c
    c
    c

And now you can count from GDB!

If you do: `b sys_write` immediately after `./rungdb -k`, it fails with `KGDB: BP remove failed: <address>`. I think this is because it would break too early on the boot sequence, and KGDB is not yet ready.

See also:

- <https://github.com/torvalds/linux/blob/v4.9/Documentation/DocBook/kgdb.tmpl>
- <https://stackoverflow.com/questions/22004616/qemu-kernel-debugging-with-kgdb/44197715#44197715>

### KGDB kernel modules

In QEMU:

    /kgdb-mod.sh

In GDB:

    lx-symbols ../kernel_module-1.0/
    b fop_write
    c
    c
    c

and you now control the count.

TODO: if I `-ex lx-symbols` to the `gdb` command, just like done for QEMU `-gdb`, the kernel oops. How to automate this step?

### KDB

If you modify `runqemu` to use:

    -append kgdboc=kbd

instead of `kgdboc=ttyS0,115200`, you enter a different debugging mode called KDB.

Usage: in QEMU:

    [0]kdb> go

Boot finishes, then:

    /kgdb.sh

And you are back in KDB. Now you can:

    [0]kdb> help
    [0]kdb> bp sys_write
    [0]kdb> go

And you will break whenever `sys_write` is hit.

The other KDB commands allow you to instruction steps, view memory, registers and some higher level kernel runtime data.

But TODO I don't think you can see where you are in the kernel source code and line step as from GDB, since the kernel source is not available on guest (ah, if only debugging information supported full source).

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

## Table of contents

1.  [Introduction](introduction.md)
1.  [Build](build.md)
1.  [kmod](kmod.md)
1.  [vermagic](vermagic.md)
1.  [ftrace](ftrace.md)
1.  [Device tree](device-tree.md)
1.  [Bibliography](bibliography.md)
1.  Examples
    1.  [Host](host/)
    1.  [QEMU Buildroot](kernel_module/)
