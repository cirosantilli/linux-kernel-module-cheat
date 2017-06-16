/*
http://www.zarb.org/~trem/kernel/pci/pci-driver.c
http://nairobi-embedded.org/linux_pci_device_driver.html
*/

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/fs.h>
#include <linux/cdev.h> /* cdev_ */
#include <asm/uaccess.h> /* put_user */

#define MAX_DEVICE  1
#define DEVICE_NAME "virtual_pci"
#define BAR_IO      0
#define BAR_MEM     3

MODULE_LICENSE("GPL");

static struct pci_device_id pci_ids[] = {
	{ PCI_DEVICE(0x1234, 0x11e8), },
	{ 0, }
};

MODULE_DEVICE_TABLE(pci, pci_ids);

static dev_t devno;
static int major;

struct pci_cdev {
	int minor;
	struct pci_dev *pci_dev;
	struct cdev *cdev;
};

static struct pci_cdev pci_cdev[MAX_DEVICE];

static void pci_cdev_del(struct pci_cdev pci_cdev[], int size, struct pci_dev *pdev)
{
	int i;

	for (i=0; i<size; i++) {
		if (pci_cdev[i].pci_dev == pdev) {
			pci_cdev[i].pci_dev = NULL;
		}
	}
}

static struct pci_dev *pci_cdev_search_pci_dev(struct pci_cdev pci_cdev[], int size, int minor)
{
	int i;
	struct pci_dev *pdev = NULL;

	for(i=0; i<size; i++) {
		if (pci_cdev[i].minor == minor) {
			pdev = pci_cdev[i].pci_dev;
			break;
		}
	}

	return pdev;
}

static struct cdev *pci_cdev_search_cdev(struct pci_cdev pci_cdev[], int size, int minor)
{
	int i;
	struct cdev *cdev = NULL;

	for (i=0; i<size; i++) {
		if (pci_cdev[i].minor == minor) {
			cdev = pci_cdev[i].cdev;
			break;
		}
	}
	return cdev;
}

/**
 * -1     => not found
 * others => found
 */
static int pci_cdev_search_minor(struct pci_cdev pci_cdev[],
		int size, struct pci_dev *pdev)
{
	int i, minor = -1;

	for (i=0; i<size; i++) {
		if (pci_cdev[i].pci_dev == pdev) {
			minor = pci_cdev[i].minor;
			break;
		}
	}
	return minor;
}

static int pci_open(struct inode *inode, struct file *file)
{
	int minor = iminor(inode);
	file->private_data = (void *)pci_cdev_search_pci_dev(pci_cdev, MAX_DEVICE, minor);
	return 0;
}

static ssize_t pci_read(struct file *file,
			char *buffer,
			size_t length,
			loff_t * offset)
{
	int byte_read = 0;
	unsigned char value;
	struct pci_dev *pdev = (struct pci_dev *)file->private_data;
	unsigned long pci_io_addr = 0;

	pci_io_addr = pci_resource_start(pdev,BAR_IO);
	while (byte_read < length) {
		value = inb(pci_io_addr + 1);
		put_user(value, &buffer[byte_read]);
		byte_read++;
	}
	return byte_read;
}

static ssize_t pci_write(struct file *filp, const char *buffer, size_t len, loff_t * off) {
	int i;
	unsigned char value;
	struct pci_dev *pdev = (struct pci_dev *)filp->private_data;
	unsigned long pci_io_addr = 0;

	pci_io_addr = pci_resource_start(pdev,BAR_IO);
	for (i=0; i<len; i++) {
		value = (unsigned char)buffer[i];
		outb(pci_io_addr+2, value);
	}
	return len;
}

static struct file_operations pci_ops = {
	.owner   = THIS_MODULE,
	.read    = pci_read,
	.write   = pci_write,
	.open    = pci_open,
};

/**
 * 0 => this driver doesn't handle this device
 * 1 => this driver handles this device
 */
static int pci_probe(struct pci_dev *dev, const struct pci_device_id *id)
{
	int i, ret, minor;
	struct cdev *cdev;
	dev_t devno;

	pr_info("pci_probe\n");

	minor = -1;
	for (i=0; i<MAX_DEVICE; i++) {
		if (pci_cdev[i].pci_dev == NULL) {
			pci_cdev[i].pci_dev = dev;
			minor = pci_cdev[i].minor;
			break;
		}
	}
	if (minor < 0) {
		dev_info(&(dev->dev), "error pci_cdev_add");
		goto error;
	}

	devno = MKDEV(major, minor);
	cdev = cdev_alloc();
	cdev_init(cdev, &pci_ops);
	cdev->owner = THIS_MODULE;

	/* register cdev */
	ret = cdev_add(cdev, devno, 1);
	if (ret < 0) {
		dev_err(&(dev->dev), "Can't register character device\n");
		goto error;
	}
	pci_cdev[minor].cdev = cdev;

	dev_info(&(dev->dev), "%s The major device number is %d (%d).\n",
	       "Registeration is a success", MAJOR(devno), MINOR(devno));
	dev_info(&(dev->dev), "If you want to talk to the device driver,\n");
	dev_info(&(dev->dev), "you'll have to create a device file. \n");
	dev_info(&(dev->dev), "We suggest you use:\n");
	dev_info(&(dev->dev), "mknod %s c %d %d\n", DEVICE_NAME, MAJOR(devno), MINOR(devno));
	dev_info(&(dev->dev), "The device file name is important, because\n");
	dev_info(&(dev->dev), "the ioctl program assumes that's the\n");
	dev_info(&(dev->dev), "file you'll use.\n");

	/* enable the device */
	pci_enable_device(dev);

	/* 'alloc' IO to talk with the card */
	if (pci_request_region(dev, BAR_IO, "IO-pci")) {
		dev_err(&(dev->dev), "Can't request BAR0\n");
		cdev_del(cdev);
		goto error;
	}

	/* TODO */
	/* check that BAR_IO is *really* IO region */
	/*if ((pci_resource_flags(dev, BAR_IO) & IORESOURCE_IO) != IORESOURCE_IO) {*/
		/*dev_err(&(dev->dev), "BAR2 isn't an IO region\n");*/
		/*cdev_del(cdev);*/
		/*goto error;*/
	/*}*/

	return 0;

error:
	return 1;
}

static void pci_remove(struct pci_dev *dev)
{
	int minor;
	struct cdev *cdev;

	minor = pci_cdev_search_minor(pci_cdev, MAX_DEVICE, dev);
	cdev = pci_cdev_search_cdev(pci_cdev, MAX_DEVICE, minor);
	if (cdev != NULL)
		cdev_del(cdev);
	pci_cdev_del(pci_cdev, MAX_DEVICE, dev);
	pci_release_region(dev, BAR_IO);
}

static struct pci_driver pci_driver = {
	.name     = "pci",
	.id_table = pci_ids,
	.probe    = pci_probe,
	.remove   = pci_remove,
};

static int __init pci_init_module(void)
{
	int i, first_minor, ret;

	ret = alloc_chrdev_region(&devno, 0, MAX_DEVICE, "lkmc_pci");
	major = MAJOR(devno);
	first_minor = MINOR(devno);
	for (i=0; i < MAX_DEVICE; i++) {
		pci_cdev[i].minor   = first_minor++;
		pci_cdev[i].pci_dev = NULL;
		pci_cdev[i].cdev    = NULL;
	}
	ret = pci_register_driver(&pci_driver);
	return 0;
}

static void pci_exit_module(void)
{
	int i;

	pci_unregister_driver(&pci_driver);
	for(i=0; i< MAX_DEVICE; i++) {
		if (pci_cdev[i].pci_dev != NULL) {
			cdev_del(pci_cdev[i].cdev);
		}
	}
	unregister_chrdev_region(devno, MAX_DEVICE);
}

module_init(pci_init_module);
module_exit(pci_exit_module);
