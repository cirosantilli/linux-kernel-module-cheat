/*
    insmod /vermagic.ko
    # => 4.9.6 SMP mod_unload modversions

TODO how to get the vermagic from running kernel from userland?
<https://lists.kernelnewbies.org/pipermail/kernelnewbies/2012-October/006306.html>
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/vermagic.h>

static int myinit(void)
{
	pr_info(__FILE__ "\n");
	return 0;
}

static void myexit(void) {}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
