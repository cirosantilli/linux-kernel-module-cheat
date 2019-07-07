/* https://cirosantilli.com/linux-kernel-module-cheat#sleep */

#include <linux/delay.h> /* usleep_range */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h> /* atomic_t */

static u32 n = 5;
module_param(n, int, S_IRUSR | S_IWUSR);

static int myinit(void)
{
	u32 i;
	for (i = 0; i < n; ++i) {
		pr_info("%d\n", i);
		usleep_range(1000000, 1000001);
	}
	return 0;
}

static void myexit(void) {}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
