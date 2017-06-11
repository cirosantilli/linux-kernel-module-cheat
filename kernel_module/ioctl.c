/*
Give integer and a pointer to the kernel, and get one positive integer out.

Feels like an archaic API... so many weird restrictions for something that could be so simple!

TODO: must the ioctl numbers be globally unique? How to ensure that?

Ioctl is super picky about the ioctl numbers, it is very annoying: https://stackoverflow.com/questions/10071296/ioctl-is-not-called-if-cmd-2

See how do_vfs_ioctl highjacks several values. This "forces" use to use the _IOx macros...

Documentation/ioctl/ioctl-number.txt has some info:

    _IO    an ioctl with no parameters
    _IOW   an ioctl with write parameters (copy_from_user)
    _IOR   an ioctl with read parameters  (copy_to_user)
    _IOWR  an ioctl with both write and read parameters.
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
