/* https://github.com/cirosantilli/linux-kernel-module-cheat#module_info */

#include <linux/module.h>
#include <linux/kernel.h>

static int myinit(void)
{
	/* Set by default based on the module file name. */
	pr_info("name = %s\n", THIS_MODULE->name);
	pr_info("version = %s\n", THIS_MODULE->version);
	/* ERROR: nope, not part of struct module. */
	/*pr_info("asdf = %s\n", THIS_MODULE->asdf);*/
	return 0;
}

static void myexit(void) {}

module_init(myinit)
module_exit(myexit)
MODULE_INFO(asdf, "qwer");
MODULE_VERSION("1.0");
MODULE_LICENSE("GPL");
