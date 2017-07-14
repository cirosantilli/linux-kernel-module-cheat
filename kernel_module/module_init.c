/*
https://stackoverflow.com/questions/3218320/what-is-the-difference-between-module-init-and-init-module-in-a-linux-kernel-mod

Hello world with direct init_module and cleantup_module.

This appears to be an older method that still works but has some drawbacks.

vs module_init and module_exit?

- modprobe only works with the module_init / module_exit. Try "modprobe module_init".
*/

#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");

int init_module(void)
{
	pr_info("init_module\n");
	return 0;
}

void cleanup_module(void)
{
	pr_info("cleanup_module\n");
}
