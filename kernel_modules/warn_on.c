/* https://cirosantilli.com/linux-kernel-module-cheat#warn_on */

#include <linux/module.h>
#include <linux/kernel.h>

static int myinit(void)
{
	pr_info("warn_on init\n");
	WARN_ON("warn_on do it");
	pr_info("warn_on after\n");
	return 0;
}

static void myexit(void)
{
	pr_info("warn_on cleanup\n");
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
