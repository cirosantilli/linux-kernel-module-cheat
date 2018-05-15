#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h> /* copy_from_user, copy_to_user */

#define NAME "lkmc_irq"
#define MAX_IRQS 256

static int irqs[MAX_IRQS];
static int major;

/**
 * Return value from kernel docs:*
 *
 * enum irqreturn
 * @IRQ_NONE        interrupt was not from this device or was not handled
 * @IRQ_HANDLED     interrupt was handled by this device
 * @IRQ_WAKE_THREAD handler requests to wake the handler thread
 */
static irqreturn_t handler(int irq, void *dev)
{
	pr_info("handler irq = %d dev = %d\n", irq, *(int *)dev);
	return IRQ_NONE;
}

static const struct file_operations fops;

static int myinit(void)
{
	int ret, i;

	major = register_chrdev(0, NAME, &fops);
	for (i = 0; i < MAX_IRQS; ++i) {
		ret = request_irq(
			i,
			handler,
			IRQF_SHARED,
			"myirqhandler0",
			&major
		);
		irqs[i] = ret;
		pr_info("request_irq irq = %d ret = %d\n", i, ret);
	}
	return 0;
}

static void myexit(void)
{
	int i;

	for (i = 0; i < MAX_IRQS; ++i) {
		if (!irqs[i]) {
			free_irq(i, &major);
		}
	}
	unregister_chrdev(major, NAME);
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
