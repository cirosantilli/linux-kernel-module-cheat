# Build

The module building system.

Kernel modules are built using a makefile located at:

	/lib/modules/$(uname -r)/build

## includes

Header files come from the same directory as the makefile: `/lib/modules/$(uname -r)/build`.

TODO how is that different from: `/usr/src/linux-headers-$(uname -r)/` ?

Those come directly from the kernel source tree.
