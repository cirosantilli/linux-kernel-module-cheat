/*
https://stackoverflow.com/questions/6139493/how-convert-char-to-int-in-linux-kernel/49811658#49811658

Usage:

	/kstrto.sh
*/

#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <uapi/linux/stat.h> /* S_IRUSR */

static struct dentry *toplevel_file;

static ssize_t write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	int ret;
	unsigned long long res;
	ret = kstrtoull_from_user(buf, len, 10, &res);
	if (ret) {
		/* Negative error code. */
		pr_info("ko = %d\n", ret);
		return ret;
	} else {
		pr_info("ok  = %llu\n", res);
		*off= len;
		return len;
	}
}

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.write = write,
};

static int myinit(void)
{
	toplevel_file = debugfs_create_file("lkmc_kstrto", S_IWUSR, NULL, NULL, &fops);
	if (!toplevel_file) {
		return -1;
	}
	return 0;
}

static void myexit(void)
{
	debugfs_remove(toplevel_file);
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
