/*
It will happen eventually, so you might as well learn do deal with it.

TODO: how to scroll up to see full trace? Shift + Page Up does not work as it normally does:
https://superuser.com/questions/848412/scrolling-up-the-failed-screen-with-kernel-panic

The alternative is to get the serial data out streamed to console or to a file:

- https://superuser.com/questions/269228/write-qemu-booting-virtual-machine-output-to-a-file
- http://www.reactos.org/wiki/QEMU#Redirect_to_a_file
*/

#include <linux/module.h>
#include <linux/kernel.h>

static int myinit(void)
{
	pr_info("panic init\n");
	panic("hello panic");
	return 0;
}

static void myexit(void)
{
	pr_info("panic cleanup\n");
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
