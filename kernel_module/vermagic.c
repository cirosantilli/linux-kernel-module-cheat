/* https://github.com/cirosantilli/linux-kernel-module-cheat#vermagic */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/vermagic.h>

static int myinit(void)
{
	pr_info("VERMAGIC_STRING = " VERMAGIC_STRING "\n");
	/* Nice try, but it is not a member. */
	/*pr_info("THIS_MODULE->vermagic = %s\n", THIS_MODULE->vermagic);*/
	return 0;
}

static void myexit(void) {}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
