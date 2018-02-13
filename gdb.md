# GDB step debugging

To GDB step debug the Linux kernel, first run:

    ./run -d

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

See also:

- <http://stackoverflow.com/questions/11408041/how-to-debug-the-linux-kernel-with-gdb-and-qemu/33203642#33203642>
- <http://stackoverflow.com/questions/4943857/linux-kernel-live-debugging-how-its-done-and-what-tools-are-used/42316607#42316607>

`O=0` is an impossible dream, `O=2` being the default: <https://stackoverflow.com/questions/29151235/how-to-de-optimize-the-linux-kernel-to-and-compile-it-with-o0> So get ready for some weird jumps, and `<value optimized out>` fun. Why, Linux, why.

## Kernel module debugging

Loadable kernel modules are a bit trickier since the kernel can place them at different memory locations depending on load order.

So we cannot set the breakpoints before `insmod`.

However, the Linux kernel GDB scripts offer the `lx-symbols` command, which takes care of that beautifully for us:

    ./run -d
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

See also: <http://stackoverflow.com/questions/28607538/how-to-debug-linux-kernel-modules-with-qemu/44095831#44095831>

### Bypassing lx-symbols

Useless, but a good way to show how hardcore you are. From inside QEMU:

    insmod /fops.ko
    cat /proc/modules

This will give a line of form:

    fops 2327 0 - Live 0xfffffffa00000000

And then tell GDB where the module was loaded with:

    Ctrl + C
    add-symbol-file ../kernel_module-1.0/fops.ko 0xfffffffa00000000

## Debug kernel early boot

TODO: why can't we break at early startup stuff such as:

    ./rungdb extract_kernel
    ./rungdb main

See also: <https://stackoverflow.com/questions/2589845/what-are-the-first-operations-that-the-linux-kernel-executes-on-boot>

## call

GDB can call functions as explained at: <https://stackoverflow.com/questions/1354731/how-to-evaluate-functions-in-gdb>

However this is failing for us:

- some symbols are not visible to `call` even though `b` sees them
- for those that are, `call` fails with an E14 error

E.g.: if we break on `sys_write` on `/count.sh`:

    >>> call printk(0, "asdf")
    Could not fetch register "orig_rax"; remote failure reply 'E14'
    >>> b printk
    Breakpoint 2 at 0xffffffff81091bca: file kernel/printk/printk.c, line 1824.
    >>> call fdget_pos(fd)
    No symbol "fdget_pos" in current context.
    >>> b fdget_pos
    Breakpoint 3 at 0xffffffff811615e3: fdget_pos. (9 locations)
    >>>

even though `fdget_pos` is the first thing `sys_write` does:

    581 SYSCALL_DEFINE3(write, unsigned int, fd, const char __user *, buf,
    582         size_t, count)
    583 {
    584     struct fd f = fdget_pos(fd);

See also: <https://github.com/cirosantilli/linux-kernel-module-cheat/issues/19>
