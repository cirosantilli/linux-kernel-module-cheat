/* https://github.com/cirosantilli/linux-kernel-module-cheat#dump_stack */

#include <linux/module.h>
#include <linux/kernel.h>

static int myinit(void)
{
	pr_info("dump_stack myinit\n");
	dump_stack();
	pr_info("dump_stack after\n");
	return 0;
}

static void myexit(void)
{
	pr_info("panic myexit\n");
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
