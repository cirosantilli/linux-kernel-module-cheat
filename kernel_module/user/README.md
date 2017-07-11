# User

Userland C programs used to test our kernel modules.

`sh` programs are simpler, and installed by copying directly with an overlay.

C programs require cross compiling, but give us more control over system calls.

These programs can also be compiled and used on host.

1.  Standalone
    1.  [myinsmod](myinsmod.c)
    1.  [myrmmod](myrmmod.c)
    1.  [init_hello](init_hello.c)
1.  Module tests
    1.  [anonymous_inode](anonymous_inode.c)
    1.  [poll](poll.c)
    1.  [ioctl](ioctl.c)
