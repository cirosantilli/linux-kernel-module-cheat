/*
Only tested in x86.

PCI driver for our minimal pci_min.c QEMU fork device.

probe already does a mmio write, which generates an IRQ and tests everything.
*/

#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>

#define BAR 0
#define CDEV_NAME "lkmc_hw_pci_min"
#define EDU_DEVICE_ID 0x11e9
#define QEMU_VENDOR_ID 0x1234

MODULE_LICENSE("GPL");

static struct pci_device_id id_table[] = {
    { PCI_DEVICE(QEMU_VENDOR_ID, EDU_DEVICE_ID), },
    { 0, }
};
MODULE_DEVICE_TABLE(pci, id_table);
static int major;
static struct pci_dev *pdev;
static void __iomem *mmio;
static struct file_operations fops = {
    .owner   = THIS_MODULE,
};

static irqreturn_t irq_handler(int irq, void *dev)
{
    pr_info("irq_handler irq = %d dev = %d\n", irq, *(int *)dev);
    iowrite32(0, mmio + 4);
    return IRQ_HANDLED;
}

static int probe(struct pci_dev *dev, const struct pci_device_id *id)
{
    pr_info("probe\n");
    major = register_chrdev(0, CDEV_NAME, &fops);
    pdev = dev;
    if (pci_enable_device(dev) < 0) {
        dev_err(&(pdev->dev), "pci_enable_device\n");
        goto error;
    }
    if (pci_request_region(dev, BAR, "myregion0")) {
        dev_err(&(pdev->dev), "pci_request_region\n");
        goto error;
    }
    mmio = pci_iomap(pdev, BAR, pci_resource_len(pdev, BAR));
    pr_info("dev->irq = %u\n", dev->irq);
    if (request_irq(dev->irq, irq_handler, IRQF_SHARED, "pci_irq_handler0", &major) < 0) {
        dev_err(&(dev->dev), "request_irq\n");
        goto error;
    }
    iowrite32(0x12345678, mmio);
    return 0;
error:
    return 1;
}

static void remove(struct pci_dev *dev)
{
    pr_info("remove\n");
    free_irq(dev->irq, &major);
    pci_release_region(dev, BAR);
    unregister_chrdev(major, CDEV_NAME);
}

static struct pci_driver pci_driver = {
    .name     = CDEV_NAME,
    .id_table = id_table,
    .probe    = probe,
    .remove   = remove,
};

static int myinit(void)
{
    if (pci_register_driver(&pci_driver) < 0) {
        return 1;
    }
    return 0;
}

static void myexit(void)
{
    pci_unregister_driver(&pci_driver);
}

module_init(myinit);
module_exit(myexit);
