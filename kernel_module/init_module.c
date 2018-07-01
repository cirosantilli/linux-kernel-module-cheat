/* https://github.com/cirosantilli/linux-kernel-module-cheat#init_module */

#include <linux/module.h>
#include <linux/kernel.h>

int init_module(void)
{
	pr_info("init_module\n");
	return 0;
}

void cleanup_module(void)
{
	pr_info("cleanup_module\n");
}
MODULE_LICENSE("GPL");
