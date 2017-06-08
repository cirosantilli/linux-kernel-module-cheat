/*
Give integer and a pointer to the kernel, and get one positive integer out.
*/

#include <linux/debugfs.h>
#include <linux/module.h>
#include <linux/printk.h> /* printk */

MODULE_LICENSE("GPL");

static struct dentry *dir;

static long unlocked_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	pr_info("cmd = %u\n", cmd);
	return cmd + 1;
}

static const struct file_operations fops = {
	.unlocked_ioctl = unlocked_ioctl
};

static int myinit(void)
{
	dir = debugfs_create_dir("lkmc_ioctl", 0);
	debugfs_create_file("f", 0666, dir, NULL, &fops);
	return 0;
}

static void myexit(void)
{
	debugfs_remove_recursive(dir);
}

module_init(myinit)
module_exit(myexit)
