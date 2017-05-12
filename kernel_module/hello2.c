/*
Hello world module 2.

Mostly to check that our build infrastructure can handle more than one module!
*/

#include <linux/module.h>
#include <linux/kernel.h>

int init_module(void)
{
	printk(KERN_INFO "hello2 init\n");
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "hello2 cleanup\n");
}
