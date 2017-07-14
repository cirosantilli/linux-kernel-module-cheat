/*
If you have the entire read output upfront, single_open
is an even more convenient version of seq_file.

This example behaves like a file that contains:

	ab
	cd
*/

#include <asm/uaccess.h> /* copy_from_user, copy_to_user */
#include <linux/debugfs.h>
#include <linux/errno.h> /* EFAULT */
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/printk.h> /* pr_info */
#include <linux/seq_file.h> /* seq_read, seq_lseek, single_release */
#include <uapi/linux/stat.h> /* S_IRUSR */

static struct dentry *debugfs_file;

static int show(struct seq_file *m, void *v)
{
	seq_printf(m, "ab\ncd\n");
	return 0;
}

static int open(struct inode *inode, struct  file *file)
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
	debugfs_file = debugfs_create_file(
		"lkmc_seq_file_single", S_IRUSR, NULL, NULL, &fops);
	return 0;
}

static void myexit(void)
{
	debugfs_remove(debugfs_file);
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
