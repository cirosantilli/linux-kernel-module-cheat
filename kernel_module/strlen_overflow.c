/*
https://www.reddit.com/r/hacking/comments/8h4qxk/what_a_buffer_overflow_in_the_linux_kernel_looks/
*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>

static int myinit(void)
{
	char buf[] = {'p', 'w', 'n'};
	pr_info("%llu\n", (long long unsigned)strlen(buf));
	return 0;
}

static void myexit(void) {}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
