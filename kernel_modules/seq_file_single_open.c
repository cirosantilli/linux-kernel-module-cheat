/* https://cirosantilli.com/linux-kernel-module-cheat#seq_file-single_open */

#include <linux/debugfs.h>
#include <linux/errno.h> /* EFAULT */
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/printk.h> /* pr_info */
#include <linux/seq_file.h> /* seq_read, seq_lseek, single_release */
#include <linux/uaccess.h> /* copy_from_user, copy_to_user */
#include <uapi/linux/stat.h> /* S_IRUSR */

static struct dentry *debugfs_file;

static int show(struct seq_file *m, void *v)
{
	seq_printf(m, "ab\ncd\n");
	return 0;
}

static int open(struct inode *inode, struct file *file)
{
	return single_open(file, show, NULL);
}

static const struct file_operations fops = {
	.llseek = seq_lseek,
	.open = open,
	.owner = THIS_MODULE,
	.read = seq_read,
	.release = single_release,
};

static int myinit(void)
{
	debugfs_file = debugfs_create_file("lkmc_seq_file_single_open", S_IRUSR, NULL, NULL, &fops);
	return 0;
}

static void myexit(void)
{
	debugfs_remove(debugfs_file);
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
