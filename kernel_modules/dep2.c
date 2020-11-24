/* https://cirosantilli.com/linux-kernel-module-cheat#kernel-module-dependencies */

#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>

extern u32 lkmc_dep;
static char *debugfs_filename = "lkmc_dep2";

static int myinit(void)
{
	debugfs_create_u32(debugfs_filename, S_IRUSR | S_IWUSR, NULL, &lkmc_dep);
	return 0;
}

static void myexit(void)
{
	debugfs_remove(debugfs_lookup(debugfs_filename, NULL));
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
