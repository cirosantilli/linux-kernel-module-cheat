/*
https://stackoverflow.com/questions/4839024/how-to-find-the-version-of-a-compiled-kernel-module/42556565#42556565

    insmod /module_version.ko
    cat /sys/modules/module_version/version
    # => 1.0
    cat /sys/module/module_version/srcversion
    # => AB0F06618BC3A36B687CDC5
    modinfo /module_version.ko | grep -E '^(src|)version'
    # => version:        1.0
    # => srcversion:     AB0F06618BC3A36B687CDC5
*/

#include <linux/module.h>
#include <linux/kernel.h>

static int myinit(void)
{
	pr_info(__FILE__ "\n");
	return 0;
}

static void myexit(void) {}

module_init(myinit)
module_exit(myexit)
MODULE_VERSION("1.0");
MODULE_LICENSE("GPL");
