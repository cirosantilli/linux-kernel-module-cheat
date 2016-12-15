# Host

Simple things that can be demonstrated by inserting a module into the currently running host. Tested on Ubuntu 16.04.

1. [hello](hello.c)
1. [ins_rm_mod.c](ins_rm_mod.c)

## Rationale

This method easier to setup, but it is not recommended for development, as:

- it may break your system
- you can't control which kernel version to use

Use VMs instead.

## Usage

We only use it for super simple examples.

Build, insert and remove a hello world module:

    make

    sudo insmod hello.ko

    # Our module should be there.
    sudo lsmod | grep hello

    # Last message should be: init_module
    dmest -T

    sudo rmmod hello

    # Last message should be: cleanup_module
    dmest -T

Insert and remove the `hello.ko` module from a C program with system calls:

	sudo ./ins_rm_mod.out
