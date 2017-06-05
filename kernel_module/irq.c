/*
TODO: get handler running multiple times on some existing interrupt from /proc/interrupts.
*/

#include <linux/interrupt.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

/**
 * Return value from kernel docs:*
 *
 * enum irqreturn
 * @IRQ_NON         interrupt was not from this device or was not handled
 * @IRQ_HANDLED     interrupt was handled by this device
 * @IRQ_WAKE_THREAD handler requests to wake the handler thread
 */
static irqreturn_t handler(int i, void *v)
{
	pr_info("handler %llu\n", (unsigned long long)jiffies);
	return IRQ_HANDLED;
}

int init_module(void)
{
	irqreturn_t r;
	r = request_irq(
		1,
		handler,
		IRQF_SHARED,
		"myirqhandler0",
		0
	);
	pr_info("request_irq %llu\n", (unsigned long long)r);
	return 0;
}

void cleanup_module(void)
{
}
