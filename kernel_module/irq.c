/*
Brute force monitor every shared interrupt that will accept us.

I.e. only works if for IRQs for which the other handlers are registered as IRQF_SHARED.

Usage:

1.  insmod and then use the keyboard and mouse. Those handlers are shared,
	and the messages show up whenever you do something.

	Does not work in text mode of course.

2.  insmod some module that generates interrupts after insmod irq.,
	e.g. pci_min or platform_device.

	TODO: why does insmod in ARM lead to infinitely many interrupts handler irq = 45 dev = 252
	and blocks the board? Is the ARM timer shared, and x86 isn't?

TODO: properly understand how each IRQ maps to what.

The Linux kernel mainline also has dummy-irq for monitoring a single IRQ.

Inside QEMU, also try:

	watch -n 1 cat /proc/interrupts

Then see how clicking the mouse and keyboard affect the interrupts. This will point you to:

- 1: keyboard
- 12: mouse click and drags
*/

#include <asm/uaccess.h> /* copy_from_user, copy_to_user */
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>

#define NAME "lkmc_character_device"
#define MAX_IRQS 256

MODULE_LICENSE("GPL");

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
