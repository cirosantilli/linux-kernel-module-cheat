/*
Hello world module 2.

Mostly to check that our build infrastructure can handle more than one module!
*/

#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");

static int myinit(void)
{
	printk(KERN_INFO "hello2 init\n");
	return 0;
}

static void myexit(void)
{
	printk(KERN_INFO "hello2 exit\n");
}

module_init(myinit)
module_exit(myexit)
