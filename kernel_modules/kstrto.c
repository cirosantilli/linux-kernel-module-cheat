/* https://github.com/cirosantilli/linux-kernel-module-cheat#kstrto */

#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h> /* copy_from_user, copy_to_user */
#include <uapi/linux/stat.h> /* S_IWUSR */

static struct dentry *toplevel_file;
static char read_buf[1024];

static int show(struct seq_file *m, void *v)
{
	seq_printf(m, read_buf);
	return 0;
}

static int open(struct inode *inode, struct file *file)
{
	return single_open(file, show, NULL);
}

static ssize_t write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	ssize_t ret;
	int kstrto_return;
	unsigned long long kstrto_result;
	kstrto_return = kstrtoull_from_user(buf, len, 10, &kstrto_result);
	if (kstrto_return) {
		/* Negative error code. */
		ret = kstrto_return;
	} else {
		ret = len;
	}
	snprintf(read_buf, sizeof(read_buf), "%llu", kstrto_result + 1);
	return ret;
}

static const struct file_operations fops = {
	.llseek = seq_lseek,
	.open = open,
	.owner = THIS_MODULE,
	.read = seq_read,
	.release = single_release,
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
