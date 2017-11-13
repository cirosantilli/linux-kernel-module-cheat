/*
    insmod /vermagic_fail.ko
    # => insmod: can't insert '/vermagic_fail.ko': invalid module format

    modinfo /vermagic_fail.ko | grep vermagic
    # => vermagic:       asdfqwer
    # => vermagic:       4.9.6 SMP mod_unload modversions

kmod `modprobe` has a flag to skip the check:

	--force-modversion

Looks like it just strips `modversion` information from the module before loading, and then the kernel skips the check.
*/

#include <linux/module.h>
#include <linux/kernel.h>

static int myinit(void)
{
	pr_info("vermagic_fail\n");
	return 0;
}

static void myexit(void) {}

module_init(myinit)
module_exit(myexit)
MODULE_INFO(vermagic, "asdfqwer");
MODULE_LICENSE("GPL");
