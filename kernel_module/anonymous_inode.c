/*
https://stackoverflow.com/questions/4508998/what-is-anonymous-inode

anon_inode_getfd example:

- get an anonymous inode via ioctl from a debugfs entry
- read jiffies from that inode

This method allows getting multiple file descriptors from a single filesystem,
which reduces namespace pollution.
*/

#include <asm/uaccess.h> /* copy_from_user, copy_to_user */
#include <linux/anon_inodes.h>
#include <linux/debugfs.h>
#include <linux/errno.h> /* EFAULT */
#include <linux/fs.h>
#include <linux/jiffies.h>
#include <linux/kernel.h> /* min */
#include <linux/module.h>
#include <linux/printk.h> /* printk */

#include "anonymous_inode.h"

MODULE_LICENSE("GPL");

static struct dentry *debugfs_file;

static ssize_t read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	char kbuf[1024];
	size_t ret;

	ret = snprintf(kbuf, sizeof(kbuf), "%llu", (unsigned long long)jiffies);
	if (copy_to_user(buf, kbuf, ret)) {
		ret = -EFAULT;
	}
	return ret;
}

static const struct file_operations fops_anon = {
	.read = read,
};

static long unlocked_ioctl(struct file *filp, unsigned int cmd, unsigned long argp)
{
	int fd;

	switch (cmd) {
		case LKMC_ANONYMOUS_INODE_GET_FD:
			fd = anon_inode_getfd(
				"random",
				&fops_anon,
				NULL,
				O_RDONLY | O_CLOEXEC
			);
			if (copy_to_user((void __user *)argp, &fd, sizeof(fd))) {
				return -EFAULT;
			}
		break;
		default:
			return -EINVAL;
		break;
	}
	return 0;
}

static const struct file_operations fops_ioctl = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = unlocked_ioctl
};

static int myinit(void)
{
	debugfs_file = debugfs_create_file("lkmc_anonymous_inode", 0, NULL, NULL, &fops_ioctl);
	return 0;
}

static void myexit(void)
{
	debugfs_remove(debugfs_file);
}

module_init(myinit)
module_exit(myexit)
