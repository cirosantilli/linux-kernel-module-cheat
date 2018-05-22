#include <linux/cdev.h> /* cdev_ */
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/uaccess.h> /* put_user */

#define BAR 0
#define CDEV_NAME "lkmc_pci"
#define EDU_DEVICE_ID 0x11e8
#define QEMU_VENDOR_ID 0x1234

/* Registers. */
#define IO_IRQ_STATUS 0x24
#define IO_IRQ_ACK 0x64
#define IO_DMA_SRC 0x80
#define IO_DMA_DST 0x88
#define IO_DMA_CNT 0x90
#define IO_DMA_CMD 0x98

/* Constants. */
/* TODO what is this magic value for? Can't it be always deduced from the direction? */
#define DMA_BASE 0x40000
/* Must give this for the DMA command to to anything. */
#define DMA_CMD 0x1
/* If given, device -> RAM. Otherwise: RAM -> dev. */
#define DMA_FROM_DEV 0x2
/* If given, raise an IRQ, and write 100 to the IRQ status register. */
#define DMA_IRQ 0x4

static struct pci_device_id pci_ids[] = {
	{ PCI_DEVICE(QEMU_VENDOR_ID, EDU_DEVICE_ID), },
	{ 0, }
};
MODULE_DEVICE_TABLE(pci, pci_ids);

static int major;
static struct pci_dev *pdev;
static void __iomem *mmio;

static irqreturn_t irq_handler(int irq, void *dev)
{
	int devi;
	irqreturn_t ret;
	u32 irq_status;

	devi = *(int *)dev;
	if (devi == major) {
		irq_status = ioread32(mmio + IO_IRQ_STATUS);
		pr_info("irq_handler irq = %d dev = %d irq_status = %llx\n",
				irq, devi, (unsigned long long)irq_status);
		/* Must do this ACK, or else the interrupts just keeps firing. */
		iowrite32(irq_status, mmio + IO_IRQ_ACK);
		ret = IRQ_HANDLED;
	} else {
		ret = IRQ_NONE;
	}
	return ret;
}

static ssize_t read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	ssize_t ret;
	u32 kbuf;

	if (*off % 4 || len == 0) {
		ret = 0;
	} else {
		kbuf = ioread32(mmio + *off);
		if (copy_to_user(buf, (void *)&kbuf, 4)) {
			ret = -EFAULT;
		} else {
			ret = 4;
			(*off)++;
		}
	}
	return ret;
}

static ssize_t write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	ssize_t ret;
	u32 kbuf;

	ret = len;
	if (!(*off % 4)) {
		if (copy_from_user((void *)&kbuf, buf, 4) || len != 4) {
			ret = -EFAULT;
		} else {
			iowrite32(kbuf, mmio + *off);
		}
	}
	return ret;
}

static loff_t llseek(struct file *filp, loff_t off, int whence)
{
	filp->f_pos = off;
	return off;
}

/* These fops are a bit daft since read and write interfaces don't map well to IO registers.
 *
 * One ioctl per register would likely be the saner option. But we are lazy.
 *
 * We use the fact that every IO is aligned to 4 bytes. Misaligned reads means EOF. */
static struct file_operations fops = {
	.owner   = THIS_MODULE,
	.llseek  = llseek,
	.read    = read,
	.write   = write,
};

/* https://stackoverflow.com/questions/5059501/probe-method-device-drivers/44739823#44739823
 *
 * Called just after insmod if the hardware device is connected,
 * not called otherwise.
 *
 * 0: all good
 * 1: failed
 */
static int pci_probe(struct pci_dev *dev, const struct pci_device_id *id)
{
	/* https://stackoverflow.com/questions/31382803/how-does-dev-family-functions-are-useful-while-debugging-kernel/44734857#44734857 */
	dev_info(&(dev->dev), "pci_probe\n");
	major = register_chrdev(0, CDEV_NAME, &fops);
	pdev = dev;
	if (pci_enable_device(dev) < 0) {
		dev_err(&(dev->dev), "pci_enable_device\n");
		goto error;
	}
	if (pci_request_region(dev, BAR, "myregion0")) {
		dev_err(&(dev->dev), "pci_request_region\n");
		goto error;
	}
	mmio = pci_iomap(dev, BAR, pci_resource_len(dev, BAR));

	/* IRQ setup.
	 *
	 * pci_read_config_byte(dev, PCI_INTERRUPT_LINE, &val);
	 * has a different value and does not work if we insert the PCI device
	 * after boot with device_add:
	 * https://stackoverflow.com/questions/44740254/how-to-handle-interrupts-from-a-pci-device-that-already-have-a-non-shareable-han?noredirect=1#comment76558680_44740254
	 */
	if (request_irq(dev->irq, irq_handler, IRQF_SHARED, "pci_irq_handler0", &major) < 0) {
		dev_err(&(dev->dev), "request_irq\n");
		goto error;
	}

	/* Optional sanity checks. The PCI is ready now, all of this could also be called from fops. */
	{
		unsigned i;
		u8 val;

		/* Check that we are using MEM instead of IO.
		 *
		 * In QEMU, the type is defiened by either:
		 *
		 * - PCI_BASE_ADDRESS_SPACE_IO
		 * - PCI_BASE_ADDRESS_SPACE_MEMORY
		 */
		if ((pci_resource_flags(dev, BAR) & IORESOURCE_MEM) != IORESOURCE_MEM) {
			dev_err(&(dev->dev), "pci_resource_flags\n");
			goto error;
		}

		/* 1Mb, as defined by the "1 << 20" in QEMU's memory_region_init_io. Same as pci_resource_len. */
		resource_size_t start = pci_resource_start(dev, BAR);
		resource_size_t end = pci_resource_end(dev, BAR);
		pr_info("length %llx\n", (unsigned long long)(end + 1 - start));

		/* The PCI standardized 64 bytes of the configuration space, see LDD3. */
		for (i = 0; i < 64u; ++i) {
			pci_read_config_byte(dev, i, &val);
			pr_info("config %x %x\n", i, val);
		}
		pr_info("dev->irq %x\n", dev->irq);

		/* Initial value of the IO memory. */
		for (i = 0; i < 0x28; i += 4) {
			pr_info("io %x %x\n", i, ioread32((void*)(mmio + i)));
		}

		/* DMA test.
		 *
		 * TODO:
		 *
		 * - deal with interrupts properly.
		 * - printf / gdb in QEMU source says dma_buf is not being set correctly
		 *
		 * Resources:
		 *
		 * - http://elixir.free-electrons.com/linux/v4.12/source/Documentation/DMA-API-HOWTO.txt
		 * - http://www.makelinux.net/ldd3/chp-15-sect-4
		 * - https://stackoverflow.com/questions/32592734/are-there-any-dma-linux-kernel-driver-example-with-pcie-for-fpga/44716747#44716747
		 * - https://stackoverflow.com/questions/17913679/how-to-instantiate-and-use-a-dma-driver-linux-module
		 * - https://stackoverflow.com/questions/5539375/linux-kernel-device-driver-to-dma-from-a-device-into-user-space-memory
		 * - RPI userland /dev/mem https://github.com/Wallacoloo/Raspberry-Pi-DMA-Example
		 * - https://stackoverflow.com/questions/34188369/easiest-way-to-use-dma-in-linux
		 */
		{
			dma_addr_t dma_handle_from, dma_handle_to;
			void *vaddr_from, *vaddr_to;
			enum { SIZE = 4 };

			/* RAM -> device. */
			vaddr_from = dma_alloc_coherent(&(dev->dev), 4, &dma_handle_from, GFP_ATOMIC);
			dev_info(&(dev->dev), "vaddr_from = %p\n", vaddr_from);
			dev_info(&(dev->dev), "dma_handle_from = %llx\n", (unsigned long long)dma_handle_from);
			*((volatile u32*)vaddr_from) = 0x12345678;
			iowrite32((u32)dma_handle_from, mmio + IO_DMA_SRC);
			iowrite32(DMA_BASE, mmio + IO_DMA_DST);
			iowrite32(SIZE, mmio + IO_DMA_CNT);
			iowrite32(DMA_CMD | DMA_IRQ, mmio + IO_DMA_CMD);

			/* device -> RAM. */
			vaddr_to = dma_alloc_coherent(&(dev->dev), 4, &dma_handle_to, GFP_ATOMIC);
			dev_info(&(dev->dev), "vaddr_to = %p\n", vaddr_to);
			dev_info(&(dev->dev), "dma_handle_to = %llx\n", (unsigned long long)dma_handle_to);
			/*
			iowrite32(DMA_BASE, mmio + IO_DMA_SRC);
			iowrite32((u32)dma_handle_to, mmio + IO_DMA_DST);
			iowrite32(SIZE, mmio + IO_DMA_CNT);
			iowrite32(DMA_CMD | DMA_FROM_DEV | DMA_IRQ, mmio + IO_DMA_CMD);
			dev_info(&(dev->dev), "*vaddr_to = %llx\n", (unsigned long long)(*((u32*)vaddr_to)));
			*/

			/*dma_free_coherent(&(dev->dev), SIZE, vaddr_from, dma_handle_from);*/
			/*dma_free_coherent(&(dev->dev), SIZE, vaddr_to, dma_handle_to);*/
		}
	}
	return 0;
error:
	return 1;
}

static void pci_remove(struct pci_dev *dev)
{
	pr_info("pci_remove\n");
	free_irq(pdev->irq, &major);
	pci_release_region(dev, BAR);
	unregister_chrdev(major, CDEV_NAME);
}

static struct pci_driver pci_driver = {
	.name     = "lkmc_pci",
	.id_table = pci_ids,
	.probe    = pci_probe,
	.remove   = pci_remove,
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
MODULE_LICENSE("GPL");
