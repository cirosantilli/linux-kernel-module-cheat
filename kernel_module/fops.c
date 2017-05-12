/*
dmesg stuff when fops happen.

Usage:

	/fops.sh

fops define what the kernel will do on filesystem system calls on all of
/dev, /proc, /sys, and consistute the main method of userland communication
in drivers (syscalls being the other one).

Here we use debugfs.
*/

#include <linux/debugfs.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

static struct dentry *dir = 0;

int fop_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "open\n");
	return 0;
}

ssize_t fop_read(struct file *file, char __user *buf, size_t len, loff_t *off)
{
	printk(KERN_INFO "read\n");
	return 0;
}

ssize_t fop_write(struct file *file, const char __user *buf, size_t len, loff_t *off)
{
	printk(KERN_INFO "write\n");
	return 0;
}

/*
Called on the last close:
http://stackoverflow.com/questions/11393674/why-is-the-close-function-is-called-release-in-struct-file-operations-in-the-l
*/
int fop_release (struct inode *inode, struct file *file)
{
	printk(KERN_INFO "release\n");
	return 0;
}

const struct file_operations fops = {
	.open = fop_open,
	.read = fop_read,
	.release = fop_release,
	.write = fop_write,
};

int init_module(void)
{
	struct dentry *file;
	dir = debugfs_create_dir("kernel_module_cheat", 0);
	if (!dir) {
		printk(KERN_ALERT "debugfs_create_dir failed");
		return -1;
	}
    file = debugfs_create_file("fops", 0666, dir, NULL, &fops);
	if (!file) {
		printk(KERN_ALERT "debugfs_create_file failed");
		return -1;
	}
	return 0;
}

void cleanup_module(void)
{
	debugfs_remove_recursive(dir);
}
