/*
TODO: get handler running multiple times on some existing interrupt from /proc/interrupts.
*/

#include <linux/interrupt.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

static int irq = 0;
module_param_named(i, irq, int, S_IRUSR);
MODULE_PARM_DESC(i, "irq line number");

/**
 * Return value from kernel docs:*
 *
 * enum irqreturn
 * @IRQ_NON      interrupt was not from this device or was not handled
 * @IRQ_HANDLED  interrupt was handled by this device
 * @IRQ_WAKE_THREAD handler requests to wake the handler thread
 */
static irqreturn_t handler(int irq, void *v)
{
	pr_info("handler irq = %d jiffies = %llx\n", irq, (unsigned long long)jiffies);
	return IRQ_HANDLED;
}

static int myinit(void)
{
	int ret;

	for (int i = 0; i < 128; ++i) {
		ret = request_irq(
			i,
			handler,
			IRQF_SHARED,
			"myirqhandler0",
			NULL
		);
		pr_info("request_irq irq = %d ret = %d\n", i, ret);
	}
	return 0;
}

static void myexit(void)
{
	free_irq(irq, NULL);
}

module_init(myinit)
module_exit(myexit)
