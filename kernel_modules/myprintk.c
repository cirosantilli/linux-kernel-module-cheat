/* https://github.com/cirosantilli/linux-kernel-module-cheat#pr_debug */

#include <linux/module.h>
#include <linux/kernel.h>

static int myinit(void)
{
	pr_alert("pr_alert\n");
	pr_crit("pr_crit\n");
	pr_err("pr_err");
	pr_warning("pr_warning\n");
	pr_notice("pr_notice\n");
	pr_info("pr_info\n");
	pr_debug("pr_debug\n");
	return 0;
}

static void myexit(void) { }

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
