/*
- https://stackoverflow.com/questions/4839024/how-to-find-the-version-of-a-compiled-kernel-module/42556565#42556565
- https://stackoverflow.com/questions/19467150/significance-of-this-module-in-linux-driver/49812248#49812248

Usage:

    insmod /module_info.ko
	# dmesg => name    = module_info
	# dmesg => version = 1.0
    cat /sys/module/module_info/version
    # => module_info
    modinfo /module_info.ko | grep -E '^version:'
    # => version:        1.0
*/

#include <linux/module.h>
#include <linux/kernel.h>

static int myinit(void)
{
	/* Set by default based on the module file name. */
	pr_info("name    = %s\n", THIS_MODULE->name);
	pr_info("version = %s\n", THIS_MODULE->version);
	return 0;
}

static void myexit(void) {}

module_init(myinit)
module_exit(myexit)
MODULE_VERSION("1.0");
MODULE_LICENSE("GPL");
