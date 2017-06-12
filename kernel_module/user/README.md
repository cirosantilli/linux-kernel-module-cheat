# User

Userland C programs used to test our kernel module.

`sh` programs are simpler, and installed copied directly with an overlay.

C programs require cross compiling, but give us more control over system calls.

These programs can also be compiled and used on host.

1. [myinsmod](myinsmod.c)
1. [myrmmod](myrmmod.c)
