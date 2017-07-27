/*
Yet another fops entrypoint.

https://stackoverflow.com/questions/8516021/proc-create-example-for-kernel-module

	insmod /procfs.ko
	cat /proc/lkmc_procfs

Output:

	abcd
*/

#include <linux/debugfs.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h> /* seq_read, seq_lseek, single_open, single_release */
#include <uapi/linux/stat.h> /* S_IRUSR */

static const char *filename = "lkmc_procfs";

static int show(struct seq_file *m, void *v)
{
	seq_printf(m, "abcd\n");
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
	proc_create(filename, 0, NULL, &fops);
	return 0;
}

static void myexit(void)
{
	remove_proc_entry(filename, NULL);
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
