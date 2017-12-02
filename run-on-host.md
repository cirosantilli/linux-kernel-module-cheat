# Run on host

This method runs the kernel modules directly on your host computer without a VM, and saves you the compilation time and disk usage of the virtual machine method.

It has however severe limitations, and you will soon see that the time and disk usage are well worth it:

-   can't control which kernel version and build options to use. So some of the modules will likely not compile because of kernel API changes, since [the Linux kernel does not have a stable kernel module API](https://stackoverflow.com/questions/37098482/how-to-build-a-linux-kernel-module-so-that-it-is-compatible-with-all-kernel-rele/45429681#45429681).
-   bugs can easily break you system. E.g.:
    - segfaults can trivially lead to a kernel crash, and require a reboot
    - your disk could get erased. Yes, this can also happen with `sudo` from userland. But you should not use `sudo` when developing newbie programs. And for the kernel you don't have the choice not to use `sudo`
    - even more subtle system corruption such as [not being able to rmmod](https://unix.stackexchange.com/questions/78858/cannot-remove-or-reinsert-kernel-module-after-error-while-inserting-it-without-r)
-   can't control which hardware is used, notably the CPU architecture
-   can't step debug it with GDB easily

Still interested?

    cd kernel_module
    ./make-host.sh

If the compilation of any of the C files fails because of kernel or toolchain differences that we don't control on the host, just rename it to remove the `.c` extension and try again:

    mv broken.c broken.c~
    ./build_host

Once you manage to compile, and have come to terms with the fact that this may blow up your host, try it out with:

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
