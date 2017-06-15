/*
Allows us to create device files with given file operations with mknod c X.

Usage:

	/character_device.sh

The major number determines which module owns the device file.

minor is to differentiate between multiple instances of the device,
e.g. two NVIDIA GPUs using the same kernel module.

We ask the kernel to automatically allocate a major number for us to avoid
conlicts with other devices.

Then we need to check /proc/devices to find out the assigned number,
and use that for the mknod.
*/

#include <asm/uaccess.h> /* copy_from_user, copy_to_user */
#include <linux/errno.h> /* EFAULT */
#include <linux/fs.h> /* register_chrdev, unregister_chrdev */
#include <linux/jiffies.h>
#include <linux/module.h>
#include <linux/printk.h> /* printk */
#include <uapi/linux/stat.h> /* S_IRUSR */

#define NAME "lkmc_character_device"

MODULE_LICENSE("GPL");

static int major;

static ssize_t read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	size_t ret;
	char kbuf[] = {'a', 'b', 'c', 'd'};

	ret = 0;
	if (*off == 0) {
		if (copy_to_user(buf, kbuf, sizeof(kbuf))) {
			ret = -EFAULT;
		} else {
			ret = sizeof(kbuf);
			*off = 1;
		}
	}
	return ret;
}

static const struct file_operations fops = {
	.read = read,
};

static int myinit(void)
{
	major = register_chrdev(0, NAME, &fops);
	return 0;
}

static void myexit(void)
{
	unregister_chrdev(major, NAME);
}

module_init(myinit)
module_exit(myexit)
