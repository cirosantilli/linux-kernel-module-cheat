/*
Hello world module.

	dmesg -c
	insmod hello.ko
	dmesg -c | grep 'hello init'
	rmmod hello.ko
	dmesg -c | grep 'hello cleanup'
*/

#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");

int init_module(void)
{
	printk(KERN_INFO "hello init\n");
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "hello cleanup\n");
}
