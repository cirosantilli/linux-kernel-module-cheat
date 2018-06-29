/* https://github.com/cirosantilli/linux-kernel-module-cheat#character-devices */

#include <linux/fs.h> /* register_chrdev, unregister_chrdev */
#include <linux/module.h>
#include <linux/seq_file.h> /* seq_read, seq_lseek, single_release */

#define NAME "lkmc_character_device"

static int major;

static int show(struct seq_file *m, void *v)
{
	seq_printf(m, "abcd");
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
	major = register_chrdev(0, NAME, &fops);
	return 0;
}

static void myexit(void)
{
	unregister_chrdev(major, NAME);
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
