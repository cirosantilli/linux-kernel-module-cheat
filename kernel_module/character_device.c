/*
Allows us to create device files with given file operations with mknod c X.

Usage:

	/character_device.sh

The major number determines which module owns the device file.

minor is to differentiate between multiple instances of the device,
e.g. two NVIDIA GPUs using the same kernel module.

We ask the kernel to automatically allocate a major number for us to avoid
conlicts with other devices.

Then we need to check /proc/devices to find out the assigned number,
and use that for the mknod.

- https://unix.stackexchange.com/questions/37829/understanding-character-device-or-character-special-files/371758#371758
*/

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
