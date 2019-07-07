/* https://cirosantilli.com/linux-kernel-module-cheat#kernel-module-dependencies */

#include <linux/kernel.h>
#include <linux/module.h>

u32 lkmc_dep = 42;
EXPORT_SYMBOL(lkmc_dep);

static int myinit(void)
{
	return 0;
}

static void myexit(void) {}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
