/* https://cirosantilli.com/linux-kernel-module-cheat#config_fortify_source */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/slab.h>

static int myinit(void)
{
	void *dst, *src;
	dst = kmalloc(0x10, GFP_KERNEL);
	src = kmalloc(0x1000000, GFP_KERNEL);
	memcpy(dst, src, 0x1000000);
	return 0;
}

static void myexit(void) {}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
