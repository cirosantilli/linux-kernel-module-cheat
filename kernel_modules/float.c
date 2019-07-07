/* https://cirosantilli.com/linux-kernel-module-cheat#floating-point-in-kernel-modules */

#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/fpu/api.h> /* kernel_fpu_begin, kernel_fpu_end */

/* float params are not supported, so we just go with int. */
static int myfloat = 1;
static int enable_fpu = 1;
module_param(myfloat, int, S_IRUSR | S_IWUSR);
module_param(enable_fpu, int, S_IRUSR | S_IWUSR);

static int myinit(void)
{
	if (enable_fpu) {
		kernel_fpu_begin();
	}
	if ((float)myfloat + 1.5f == 2.5f) {
		pr_info("magic value\n");
	}
	if (enable_fpu) {
		kernel_fpu_end();
	}
	return 0;
}

static void myexit(void) {}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
