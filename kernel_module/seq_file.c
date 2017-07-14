/*
Adapted from: Documentation/filesystems/seq_file.txt
but we limit the count to the max module parameter.

Writting trivial read fops is repetitive and error prone.

The seq_file API makes the process much easier for those trivial cases.

This example is behaves just like a file that contains:

	0
	1
	2

However, we only store a single integer in memory
and calculate the file on the fly in an iterator fashion.

There is not write version, as writes are more complex:
https://stackoverflow.com/questions/30710517/how-to-implement-a-writable-proc-file-by-using-seq-file-in-a-driver-module

Bibliography:

- Documentation/filesystems/seq_file.txt
- https://stackoverflow.com/questions/25399112/how-to-use-a-seq-file-in-linux-modules
*/

#include <asm/uaccess.h> /* copy_from_user, copy_to_user */
#include <linux/debugfs.h>
#include <linux/errno.h> /* EFAULT */
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/printk.h> /* pr_info */
#include <linux/seq_file.h> /* seq_read, seq_lseek, single_release */
#include <linux/slab.h> /* kmalloc, kfree */
#include <uapi/linux/stat.h> /* S_IRUSR */

static int max = 2;
module_param(max, int, S_IRUSR | S_IWUSR);

static struct dentry *debugfs_file;

/* Called at the beginning of every read.
 *
 * The return value is passsed to the first show.
 * It normally represents the current position of the iterator.
 * It could be any struct, but we use just a single integer here.
 *
 * NULL return means stop should be called next, and so the read will be empty..
 * This happens for example for an ftell that goes beyond the file size.
 */
static void *start(struct seq_file *s, loff_t *pos)
{
	loff_t *spos;

	pr_info("start pos = %llx\n", (unsigned long long)*pos);
 	spos = kmalloc(sizeof(loff_t), GFP_KERNEL);
	if (!spos || *pos >= max)
		return NULL;
	*spos = *pos;
	return spos;
}

/* The return value is passed to next show.
 * If NULL, stop is called next instead of show, and read ends.
 *
 * Can get called multiple times, until enough data is returned for the read.
 */
static void *next(struct seq_file *s, void *v, loff_t *pos)
{
	loff_t *spos;

	spos = v;
	pr_info("next pos = %llx\n", (unsigned long long)*pos);
	if (*pos >= max)
		return NULL;
	*pos = ++*spos;
	return spos;
}

/* Called at the end of every read. */
static void stop(struct seq_file *s, void *v)
{
	pr_info("stop\n");
	kfree(v);
}

/* Return 0 means success, SEQ_SKIP ignores previous prints, negative for error. */
static int show(struct seq_file *s, void *v)
{
	loff_t *spos;

	spos = v;
	pr_info("show pos = %llx\n", (unsigned long long)*spos);
	seq_printf(s, "%llx\n", (long long unsigned)*spos);
	return 0;
}

static struct seq_operations my_seq_ops = {
    .next  = next,
    .show  = show,
    .start = start,
    .stop  = stop,
};

static int open(struct inode *inode, struct file *file)
{
	pr_info("open\n");
    return seq_open(file, &my_seq_ops);
}

static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .llseek  = seq_lseek,
    .open    = open,
    .read    = seq_read,
    .release = seq_release
};

static int myinit(void)
{
	debugfs_file = debugfs_create_file(
		"lkmc_seq_file", S_IRUSR, NULL, NULL, &fops);
	if (debugfs_file) {
		return 0;
	} else {
		return -EINVAL;
	}
}

static void myexit(void)
{
	debugfs_remove(debugfs_file);
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
