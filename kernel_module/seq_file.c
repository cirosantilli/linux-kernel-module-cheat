/*
Adapted from: http://allskyee.blogspot.co.uk/2011/12/proc-seqfile-write.html

Writting trivial read fops is repetitive and error prone.

The seq_file API makes the process much easier for those trivial cases.

There is not write version however, as writes are more complex:
https://stackoverflow.com/questions/30710517/how-to-implement-a-writable-proc-file-by-using-seq-file-in-a-driver-module
*/

#include <asm/uaccess.h> /* copy_from_user, copy_to_user */
#include <linux/debugfs.h>
#include <linux/errno.h> /* EFAULT */
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/printk.h> /* pr_info */
#include <linux/seq_file.h> /* seq_read, seq_lseek, single_release */
#include <uapi/linux/stat.h> /* S_IRUSR */

MODULE_LICENSE("GPL");

static struct dentry *debugfs_file;

static void * next(struct seq_file *s, void *v, loff_t *pos)
{
	pr_info("next\n");
    (*(unsigned long *)v)++;
    (*pos)++;
    return *pos < 3 ? v : NULL;
}

static void * start(struct seq_file *s, loff_t *pos)
{
    static unsigned long counter = 0;

	pr_info("start pos = %llx\n", (unsigned long long)*pos);
    if (*pos == 0) {
        return &counter;
	} else {
        *pos = 0;
        return NULL;
    }
}

static int show(struct seq_file *s, void *v)
{
	pr_info("show\n");
    seq_printf(s, "%lx\n", *(unsigned long *)v);
    return 0;
}

static void stop(struct seq_file *s, void *v)
{
	pr_info("stop\n");
}

static struct seq_operations my_seq_ops = {
    .next  = next,
    .show  = show,
    .start = start,
    .stop  = stop,
};

static int open(struct inode *inode, struct file *file)
{
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
	debugfs_file = debugfs_create_file("lkmc_seq_file", S_IRUSR | S_IWUSR, NULL, NULL, &fops);
	return 0;
}

static void myexit(void)
{
	debugfs_remove(debugfs_file);
}

module_init(myinit)
module_exit(myexit)
