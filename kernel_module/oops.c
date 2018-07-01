/* https://github.com/cirosantilli/linux-kernel-module-cheat#kernel-panic-and-oops */

#include <linux/module.h>
#include <linux/kernel.h>

static int myinit(void)
{
	pr_info("oops myinit\n");
	*(int *)0 = 0;
	pr_info("oops after\n");
	return 0;
}

static void myexit(void)
{
	pr_info("oops myexit\n");
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
