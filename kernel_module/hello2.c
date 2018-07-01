/* https://github.com/cirosantilli/linux-kernel-module-cheat#getting-started-natively */

#include <linux/module.h>
#include <linux/kernel.h>

static int myinit(void)
{
	pr_info("hello2 init\n");
	return 0;
}

static void myexit(void)
{
	pr_info("hello2 exit\n");
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
