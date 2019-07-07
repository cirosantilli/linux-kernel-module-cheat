/* https://cirosantilli.com/linux-kernel-module-cheat#kernel-module-dependencies */

#include <linux/kernel.h>
#include <linux/module.h>

extern u32 lkmc_dep;

static int myinit(void)
{
	pr_info("%llu\n", (long long unsigned)lkmc_dep);
	return 0;
}

static void myexit(void) {}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
