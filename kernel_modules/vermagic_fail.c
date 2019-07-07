/* https://cirosantilli.com/linux-kernel-module-cheat#vermagic */

#include <linux/module.h>
#include <linux/kernel.h>

static int myinit(void)
{
	pr_info("vermagic_fail\n");
	return 0;
}

static void myexit(void) {}

module_init(myinit)
module_exit(myexit)
MODULE_INFO(vermagic, "asdfqwer");
MODULE_LICENSE("GPL");
