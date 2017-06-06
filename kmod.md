# kmod

Multi-call executable that implements: `lsmod`, `insmod`, `rmmod`, and other tools.

BusyBox also implements its own version of those executables.

Source: <https://git.kernel.org/pub/scm/utils/kernel/kmod/kmod.git>

The other tools are just symlinks to it.

## module-init-tools

Name of a predecessor set of tools.

## package version

From any of the commands, `--version`:

    modinfo --version

Package that provides utilities

## lsmod

List loaded kernel modules.

Info is taken from `/proc/modules`

    lsmod

Sample output:

    cfg80211              175574  2 rtlwifi,mac80211
    ^^^^^^^^              ^^^^^^  ^ ^^^^^^^,^^^^^^^^
    1                     2       3 4       5

1.  Name.

2.  Size.

3.  Number of running instances.

	If negative, TODO

4.  Depends on 1.

5.  Depends on 2.

To get more info:

    cat /proc/modules

Also contains two more columns:

- status: Live, Loading or Unloading
- memory offset: 0x129b0000

## modinfo

Get info about a module by filename or by module name:

    modinfo ./a.ko
    modinfo a

TODO must take a `.ko` file?

## insmod

    sudo insmod hello.ko

Loads the module.

Does not check for dependencies.

## rmmod

Remove a module. Takes either the module name or the `.ko` file:

    sudo rmmod hello
    sudo rmmod ./hello.ko

## modprobe

Vs `insmod`:

- <https://askubuntu.com/questions/20070/whats-the-difference-between-insmod-and-modprobe>
- <https://stackoverflow.com/questions/22891705/whats-the-difference-between-insmod-and-modprobe>

List available modules relative path to `/lib/modules/$KERNEL_VERSION/`:

    sudo modprobe -l

Load the module:

    sudo modprobe $m

Checks for dependencies.

Load module under different name to avoid conflicts:

    sudo modprobe vmhgfs -o vm_hgfs

Remove module:

    sudo modprobe -r $m

Check if dependencies are OK:

    sudo depmod -a

Get info about given `.ko` module file:

    m=a
    sudo rmmod $m
