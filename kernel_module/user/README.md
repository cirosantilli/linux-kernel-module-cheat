# User

Userland C programs used to test our kernel modules.

`sh` programs are simpler, and installed by copying directly with an overlay.

C programs require cross compiling, but give us more control over system calls.

These programs can also be compiled and used on host.

1.  Standalone
    1.  [myinsmod](myinsmod.c)
    1.  [myrmmod](myrmmod.c)
    1.  [usermem](usermem.c)
        1.  [pagemap_dump](pagemap_dump.c)
    1.  inits
        1.  [sleep_forever](sleep_forever.c)
        1.  [poweroff](poweroff.c)
    1.  [uio_read](uio_read.c)
1.  Module tests
    1.  [anonymous_inode](anonymous_inode.c)
    1.  [poll](poll.c)
    1.  [ioctl](ioctl.c)
