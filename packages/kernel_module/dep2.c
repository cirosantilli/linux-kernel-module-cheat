/* https://github.com/cirosantilli/linux-kernel-module-cheat#kernel-module-dependencies */

#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>

extern u32 lkmc_dep;
static struct dentry *debugfs_file;

static int myinit(void)
{
	debugfs_file = debugfs_create_u32("lkmc_dep2", S_IRUSR | S_IWUSR, NULL, &lkmc_dep);
	return 0;
}

static void myexit(void)
{
	debugfs_remove(debugfs_file);
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
