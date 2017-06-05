/*
Hello world with module_init / exit macros.

TODO: vs direct init_module and cleanup_module.

- modprobe only works with the macros. Try "modprobe module_init".
*/

#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");

static int module_init_init(void)
{
	pr_info("module_init init\n");
	return 0;
}

static void module_init_exit(void)
{
	pr_info("module_exit cleanup\n");
}

module_init(module_init_init)
module_exit(module_init_exit)
