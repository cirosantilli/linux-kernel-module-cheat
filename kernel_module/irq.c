/*
TODO: get handler running multiple times on some existing interrupt from /proc/interrupts.
*/

#include <asm/uaccess.h> /* copy_from_user, copy_to_user */
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>

#define NAME "lkmc_character_device"

MODULE_LICENSE("GPL");

static int major;
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
	pr_info("handler irq = %d\n", irq);
	return IRQ_HANDLED;
}

static const struct file_operations fops;

static int myinit(void)
{
	int ret;

	major = register_chrdev(0, NAME, &fops);
	for (int i = 0; i < 128; ++i) {
		ret = request_irq(
			i,
			handler,
			IRQF_SHARED,
			"myirqhandler0",
			&major
		);
		pr_info("request_irq irq = %d ret = %d\n", i, ret);
	}
	return 0;
}

static void myexit(void)
{
	free_irq(irq, NULL);
	unregister_chrdev(major, NAME);
}

module_init(myinit)
module_exit(myexit)
