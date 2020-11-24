/* https://cirosantilli.com/linux-kernel-module-cheat#procfs */

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

static const struct proc_ops pops = {
	.proc_lseek = seq_lseek,
	.proc_open = open,
	.proc_read = seq_read,
	.proc_release = single_release,
};

static int myinit(void)
{
	proc_create(filename, 0, NULL, &pops);
	return 0;
}

static void myexit(void)
{
	remove_proc_entry(filename, NULL);
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
