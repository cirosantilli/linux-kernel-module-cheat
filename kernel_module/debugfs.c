/*
Adapted from: https://github.com/chadversary/debugfs-tutorial/blob/47b3cf7ca47208c61ccb51b27aac6f9f932bfe0b/example1/debugfs_example1.c

Usage:

	/debugfs.sh

Requires `CONFIG_DEBUG_FS=y`.

Only the more basic fops can be implemented in debugfs, e.g. mmap is never called:

- https://patchwork.kernel.org/patch/9252557/
- https://github.com/torvalds/linux/blob/v4.9/fs/debugfs/file.c#L212
*/

#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <uapi/linux/stat.h> /* S_IRUSR */

static struct dentry *dir, *toplevel_file;
static u32 value = 42;

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
	struct dentry *file;
	dir = debugfs_create_dir("lkmc_debugfs", 0);
	if (!dir) {
		pr_alert("debugfs_create_dir failed");
		return -1;
	}
	file = debugfs_create_u32("myfile", S_IRUSR | S_IWUSR, dir, &value);
	if (!file) {
		pr_alert("debugfs_create_u32 failed");
		return -1;
	}

	/* Created on the toplevel of the debugfs mount,
	 * and with explicit fops instead of a fixed integer value. */
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
