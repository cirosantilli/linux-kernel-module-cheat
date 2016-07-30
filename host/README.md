# Host

Simple things that can be demonstrated by inserting a module into the currently running host. Tested on Ubuntu 16.04.

1. [hello](hello.c)
1. [ins_rm_mod.c](ins_rm_mod.c)

## Rationale

This method easier to setup, but it is not recommended for development, as:

- it may break your system.
- you can't control which kernel version to use

Use VMs instead.

## Usage

We only use it for super simple examples.

Build, insert and remove a hello world module:

    make ins
    make rm
    make log

The last lines of the log should contain:

    init_module
    cleanup_module

Insert and remove a module from a C program:

    make ins_rm_run
