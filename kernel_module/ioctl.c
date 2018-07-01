/* https://github.com/cirosantilli/linux-kernel-module-cheat#ioctl */

#include <linux/debugfs.h>
#include <linux/module.h>
#include <linux/printk.h> /* printk */
#include <linux/uaccess.h> /* copy_from_user, copy_to_user */

#include "ioctl.h"

static struct dentry *debugfs_file;

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
	.owner = THIS_MODULE,
	.unlocked_ioctl = unlocked_ioctl
};

static int myinit(void)
{
	/* ioctl permissions are not automatically restricted by rwx as for read / write,
	 * but we could of course implement that ourselves:
	 * https://stackoverflow.com/questions/29891803/user-permission-check-on-ioctl-command */
	debugfs_file = debugfs_create_file("lkmc_ioctl", 0, NULL, NULL, &fops);
	return 0;
}

static void myexit(void)
{
	debugfs_remove(debugfs_file);
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
