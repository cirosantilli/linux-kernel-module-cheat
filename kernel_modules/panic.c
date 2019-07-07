/* https://cirosantilli.com/linux-kernel-module-cheat#kernel-panic-and-oops */

#include <linux/module.h>
#include <linux/kernel.h>

static int myinit(void)
{
	pr_info("panic myinit\n");
	panic("hello panic");
	pr_info("panic after\n");
	return 0;
}

static void myexit(void)
{
	pr_info("panic myexit\n");
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
