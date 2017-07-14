/*
Hello world module 2.

Mostly to check that our build infrastructure can handle more than one module!
*/

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
