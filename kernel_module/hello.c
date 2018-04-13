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

static int myinit(void)
{
	pr_info("hello init\n");
	/* 0 for success, any negative value means failure,
	 * E* consts if you want to specify failure cause.
	 * https://www.linux.com/learn/kernel-newbie-corner-loadable-kernel-modules-coming-and-going */
	return 0;
}

static void myexit(void)
{
	pr_info("hello exit\n");
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
