/*
Only works for ARM.

Uses the hw/misc/lkmc_platform_device.c minimal device added in our QEMU fork.

See: https://stackoverflow.com/questions/28315265/how-to-add-a-new-device-in-qemu-source-code/44612957#44612957

Expected outcome after insmod:

- QEMU reports MMIO with printfs
- IRQs are generated and handled by this module, which logs to dmesg
*/

#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_irq.h>
#include <linux/platform_device.h>

MODULE_LICENSE("GPL");

static struct resource res;
static unsigned int irq;
static void __iomem *map;

static irqreturn_t lkmc_irq_handler(int irq, void *dev)
{
	/* TODO this 34 and not 18 as in the DTS, likely the interrupt controller moves it around.
	 * Understand precisely. 34 = 18 + 16. */
	pr_info("lkmc_irq_handler irq = %d dev = %llx\n", irq, *(unsigned long long *)dev);
	/* ACK the IRQ. */
	iowrite32(0x9ABCDEF0, map + 4);
	return IRQ_HANDLED;
}

static int lkmc_platform_device_probe(struct platform_device *pdev)
{
	int asdf;
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;

	dev_info(dev, "probe\n");

	/* Play with our custom poperty. */
	if (of_property_read_u32(np, "lkmc-asdf", &asdf) ) {
		dev_err(dev, "of_property_read_u32\n");
		return -EINVAL;
	}
	if (asdf != 0x12345678) {
		dev_err(dev, "asdf = %llx\n", (unsigned long long)asdf);
		return -EINVAL;
	}

	/* IRQ. */
	irq = irq_of_parse_and_map(dev->of_node, 0);
	if (request_irq(irq, lkmc_irq_handler, 0, "lkmc_platform_device", dev) < 0) {
		dev_err(dev, "request_irq");
		return -EINVAL;
	}
	dev_info(dev, "irq = %u\n", irq);

	/* MMIO. */
	if (of_address_to_resource(pdev->dev.of_node, 0, &res)) {
		dev_err(dev, "of_address_to_resource");
		return -EINVAL;
	}
	if  (!request_mem_region(res.start, resource_size(&res), "lkmc_platform_device")) {
		dev_err(dev, "request_mem_region");
		return -EINVAL;
	}
	map = of_iomap(pdev->dev.of_node, 0);
	if (!map) {
		dev_err(dev, "of_iomap");
		return -EINVAL;
	}
	dev_info(dev, "res.start = %u resource_size = %llx\n",
			res.start, (unsigned long long)resource_size(&res));

	/* Test MMIO and IRQ. */
	iowrite32(0x12345678, map);

	return 0;
}

static int lkmc_platform_device_remove(struct platform_device *pdev)
{
	dev_info(&pdev->dev, "remove\n");
	free_irq(irq, &pdev->dev);
	iounmap(map);
	release_mem_region(res.start, resource_size(&res));
	return 0;
}

static const struct of_device_id of_lkmc_platform_device_match[] = {
	{ .compatible = "lkmc_platform_device", },
	{},
};

MODULE_DEVICE_TABLE(of, of_lkmc_platform_device_match);

static struct platform_driver lkmc_plaform_driver = {
	.probe      = lkmc_platform_device_probe,
	.remove	    = lkmc_platform_device_remove,
	.driver     = {
		.name   = "lkmc_platform_device",
		.of_match_table = of_lkmc_platform_device_match,
		.owner = THIS_MODULE,
	},
};

static int lkmc_platform_device_init(void)
{
	pr_info("lkmc_platform_device_init\n");
	return platform_driver_register(&lkmc_plaform_driver);
}

static void lkmc_platform_device_exit(void)
{
	pr_info("lkmc_platform_device_exit\n");
	platform_driver_unregister(&lkmc_plaform_driver);
}

module_init(lkmc_platform_device_init)
module_exit(lkmc_platform_device_exit)
