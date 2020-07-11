/* https://cirosantilli.com/linux-kernel-module-cheat#kernel-module-parameters */

#include <linux/debugfs.h>
#include <linux/delay.h> /* usleep_range */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/seq_file.h> /* seq_read, seq_lseek, single_release */
#include <uapi/linux/stat.h> /* S_IRUSR | S_IWUSR */

static int i = 0;
static int j = 0;
module_param(i, int, S_IRUSR | S_IWUSR);
module_param(j, int, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(i, "my favorite int");
MODULE_PARM_DESC(j, "my second favorite int");

static struct dentry *debugfs_file;

static int show(struct seq_file *m, void *v)
{
	char kbuf[18];
	int ret;

	ret = snprintf(kbuf, sizeof(kbuf), "%d %d", i, j);
	seq_printf(m, kbuf);
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
	debugfs_file = debugfs_create_file("lkmc_params", S_IRUSR, NULL, NULL, &fops);
	return 0;
}

static void myexit(void)
{
	debugfs_remove(debugfs_file);
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
