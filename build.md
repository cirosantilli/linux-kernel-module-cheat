# Build

The module building system.

Explained at: <https://www.kernel.org/doc/Documentation/kbuild/modules.txt>

Full method: get the kernel, build it to a directory `$KDIR`, and then run:

	make -C "$KDIR" M="$(PWD)" modules

Quick method: no need for a full build, just:

    make modules_prepare

but you lose some functionality. TODO: module insert on host then fails with:

    insmod: ERROR: could not insert module hello.ko: Invalid module format

even though kernel tree was checked out to match the host.

Using your distro's kernel version:

	/lib/modules/$(uname -r)/build

## includes

Header files come from the same directory as the makefile: `/lib/modules/$(uname -r)/build`.

TODO how is that different from: `/usr/src/linux-headers-$(uname -r)/` ?

Those come directly from the kernel source tree.
