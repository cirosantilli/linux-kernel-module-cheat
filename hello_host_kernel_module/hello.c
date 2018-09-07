#include <linux/module.h>
#include <linux/kernel.h>

static int myinit(void)
{
	pr_info("hello init\n");
	return 0;
}

static void myexit(void)
{
	pr_info("hello exit\n");
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
