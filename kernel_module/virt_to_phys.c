/*
TODO not working. Tested with both:

	/virt_to_phys.sh

and on QEMU monitor:

	xp 0x<vaddr>

- https://stackoverflow.com/questions/5748492/is-there-any-api-for-determining-the-physical-address-from-virtual-address-in-li
- https://stackoverflow.com/questions/43325205/can-we-use-virt-to-phys-for-user-space-memory-in-kernel-module
*/

#include <asm/io.h> /* virt_to_phys */
#include <linux/debugfs.h>
#include <linux/delay.h> /* usleep_range */
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/seq_file.h> /* single_open, single_release */

MODULE_LICENSE("GPL");

static volatile u32 i = 0x12345678;

static struct dentry *debugfs_file;

static int show(struct seq_file *m, void *v)
{
	seq_printf(m,
		"i 0x%llx\n"
		"&i %p\n"
		"virt_to_phys 0x%llx\n",
		(unsigned long long)i,
		&i,
		(unsigned long long)virt_to_phys(&i)
	);
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
		"lkmc_virt_to_phys", S_IRUSR, NULL, NULL, &fops);
	return 0;
}

static void myexit(void)
{
	debugfs_remove(debugfs_file);
}

module_init(myinit)
module_exit(myexit)
