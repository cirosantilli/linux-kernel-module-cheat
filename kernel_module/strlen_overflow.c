#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>

static int myinit(void)
{
	enum { size = 256 };
	int i = 1;
	char buf[size];
	char buf2[size];
	for (i = 0; i < size; i++) {
		buf[i] = 'a';
		buf2[i] = 'b';
	}
	/*char buf[] = {'p', 'w', 'n'};*/
	pr_info("%llu\n", (long long unsigned)strlen(buf));
	return 0;
}

static void myexit(void) {}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
