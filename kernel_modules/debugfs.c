/* https://cirosantilli.com/linux-kernel-module-cheat#debugfs */

#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <uapi/linux/stat.h> /* S_IRUSR */

static struct dentry *dir, *toplevel_file;
static u32 value = 42;

/* This basically re-implements the write operation of debugfs_create_u32,
 * it is just an excuse to illustrate a fop. */
static ssize_t write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	int ret;
	unsigned long long res;
	/* https://stackoverflow.com/questions/6139493/how-convert-char-to-int-in-linux-kernel */
	ret = kstrtoull_from_user(buf, len, 10, &res);
	if (ret) {
		/* Negative error code. */
		return ret;
	} else {
		value = res;
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
	dir = debugfs_create_dir("lkmc_debugfs", 0);
	if (!dir) {
		pr_alert("debugfs_create_dir failed");
		return -1;
	}
	debugfs_create_u32("myfile", S_IRUSR | S_IWUSR, dir, &value);

	/* Created on the toplevel of the debugfs mount,
	 * and with explicit fops instead of a fixed integer value.
	 */
	toplevel_file = debugfs_create_file(
		"lkmc_debugfs_file", S_IWUSR, NULL, NULL, &fops);
	if (!toplevel_file) {
		return -1;
	}
	return 0;
}

static void myexit(void)
{
	debugfs_remove_recursive(dir);
	debugfs_remove(toplevel_file);
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
