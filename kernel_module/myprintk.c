#include <linux/module.h>
#include <linux/kernel.h>

static int myinit(void)
{
	pr_alert("printk alert\n");
	pr_crit("printk crit\n");
	pr_err("printk err\n");
	pr_warning("printk warning\n");
	pr_notice("printk notice\n");
	pr_info("printk info\n");
	pr_debug("printk debug\n");
	return 0;
}

static void myexit(void) { }

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
