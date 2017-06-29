/*
TODO: how to setup a device with magic addresses without PCI?
https://stackoverflow.com/questions/26965239/board-files-for-x86-based-systems

request_mem_region returns NULL.

TODO: which physical address is QEMU mapping?
*/

#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <uapi/linux/stat.h> /* S_IRUSR | S_IWUSR */

#define CDEV_NAME "lkmc_hw_min"
#define MMIO_LEN 8
#define IRQ_RAISE 0
#define IRQ_ACK 4

MODULE_LICENSE("GPL");

static unsigned long mmio_addr = 0;
module_param(mmio_addr, ulong, S_IRUSR | S_IWUSR);

static int major;
static int irq;
static void __iomem *mmio;

static struct file_operations fops = {
	.owner   = THIS_MODULE,
};

static irqreturn_t irq_handler(int irq, void *dev)
{
	pr_info("irq_handler irq = %d dev = %d\n", irq, *(int *)dev);
	iowrite32(0, mmio + IRQ_ACK);
	return IRQ_HANDLED;
}

static int myinit(void)
{
	pr_info("init\n");
	major = register_chrdev(0, CDEV_NAME, &fops);
	mmio = (void __iomem *)mmio_addr;
	if (request_mem_region(mmio_addr, MMIO_LEN, CDEV_NAME) == NULL) {
		pr_info("request_mem_region mmio = %p\n", mmio);
		goto error;
	}
	ioremap(mmio_addr, 8);
	irq = 10;
	if (request_irq(irq, irq_handler, IRQF_SHARED, CDEV_NAME, &major) < 0) {
		pr_info("request_irq\n");
		goto error;
	}
	iowrite32(0x12345678, mmio + IRQ_RAISE);
	return 0;
error:
	return -ENODEV;
}

static void myexit(void)
{
	pr_info("exit\n");
	free_irq(irq, &major);
	release_mem_region(mmio_addr, MMIO_LEN);
	unregister_chrdev(major, CDEV_NAME);
}

module_init(myinit);
module_exit(myexit);
