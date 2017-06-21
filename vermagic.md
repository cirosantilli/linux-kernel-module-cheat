# vermagic

If the module does not match that of the kernel, `insmod` is unhappy and fails.

Get it from kernel module:

    modinfo mymod.ko

Override it on module source:

    MODULE_INFO(vermagic, "newver");

On the Linux kernel 4.9.6, it is defined under `include/linux/vermagic.h`:

    #define VERMAGIC_STRING                                                 \
            UTS_RELEASE " "                                                 \
            MODULE_VERMAGIC_SMP MODULE_VERMAGIC_PREEMPT                     \
            MODULE_VERMAGIC_MODULE_UNLOAD MODULE_VERMAGIC_MODVERSIONS       \
            MODULE_ARCH_VERMAGIC

TODO can you get it from running kernel from userland? <https://lists.kernelnewbies.org/pipermail/kernelnewbies/2012-October/006306.html>

Desktop `modprobe` has a flag to skip the check:

	`--force-modversion`

Looks like it just strips `modversion` information from the module, and then the kernel skips the check.
