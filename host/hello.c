#include <linux/module.h>
#include <linux/kernel.h>

int init_module(void)
{
	printk(KERN_INFO "init_module\n");
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "cleanup_module\n");
}
