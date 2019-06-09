/* https://github.com/cirosantilli/linux-kernel-module-cheat#kernel_modules-buildroot-package */

#include <linux/module.h>
#include <linux/kernel.h>

static int myinit(void)
{
	pr_info("init buildroot\n");
	return 0;
}

static void myexit(void)
{
	pr_info("exit buildroot\n");
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
