/*
Hello world module.

	dmesg -c
	insmod hello.ko
	dmesg -c | grep 'hello init'
	rmmod hello.ko
	dmesg -c | grep 'hello exit'
*/

#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_INFO(vermagic, "4.9.6 mod_unload ");

static int myinit(void)
{
	printk(KERN_INFO "hello init\n");
	return 0;
}

static void myexit(void)
{
	printk(KERN_INFO "hello exit\n");
}

module_init(myinit)
module_exit(myexit)
