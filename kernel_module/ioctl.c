/*
Input: an integer (with some annoying restrictions) and a pointer

Output:

- positive integer return value, which for sanity should only be used with negative values for success
- the input pointer data may be overwritten to contain output

Feels like an archaic API... so many weird restrictions and types for something that could be so simple!

Documentation/ioctl/ioctl-number.txt has some info:

    _IO    an ioctl with no parameters
    _IOW   an ioctl with write parameters (copy_from_user)
    _IOR   an ioctl with read parameters  (copy_to_user)
    _IOWR  an ioctl with both write and read parameters.
*/

#include <asm/uaccess.h> /* copy_from_user, copy_to_user */
#include <linux/debugfs.h>
#include <linux/module.h>
#include <linux/printk.h> /* printk */

#include "ioctl.h"

MODULE_LICENSE("GPL");

static struct dentry *dir;

static long unlocked_ioctl(struct file *filp, unsigned int cmd, unsigned long argp)
{
	void __user *arg_user;
	union {
		int i;
		lkmc_ioctl_struct s;
	} arg_kernel;

	arg_user = (void __user *)argp;
	pr_info("cmd = %x\n", cmd);
	switch (cmd) {
		case LKMC_IOCTL_INC:
			if (copy_from_user(&arg_kernel.i, arg_user, sizeof(arg_kernel.i))) {
				return -EFAULT;
			}
			pr_info("0 arg = %d\n", arg_kernel.i);
			arg_kernel.i += 1;
			if (copy_to_user(arg_user, &arg_kernel.i, sizeof(arg_kernel.i))) {
				return -EFAULT;
			}
		break;
		case LKMC_IOCTL_INC_DEC:
			if (copy_from_user(&arg_kernel.s, arg_user, sizeof(arg_kernel.s))) {
				return -EFAULT;
			}
			pr_info("1 arg = %d %d\n", arg_kernel.s.i, arg_kernel.s.j);
			arg_kernel.s.i += 1;
			arg_kernel.s.j -= 1;
			if (copy_to_user(arg_user, &arg_kernel.s, sizeof(arg_kernel.s))) {
				return -EFAULT;
			}
		break;
		default:
			return -EINVAL;
		break;
	}
	return 0;
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
