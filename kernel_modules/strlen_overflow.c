/* https://cirosantilli.com/linux-kernel-module-cheat#config_fortify_source */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>

static int myinit(void)
{
	/* Missing terminaing NUL '\0'. */
	char buf[] = {'p', 'w', 'n'};
	pr_info("%llu\n", (long long unsigned)strlen(buf));
	return 0;
}

static void myexit(void) {}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
