/* https://github.com/cirosantilli/linux-kernel-module-cheat#virt_to_phys */

#include <asm/io.h> /* virt_to_phys */
#include <linux/debugfs.h>
#include <linux/delay.h> /* usleep_range */
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/seq_file.h> /* single_open, single_release */
#include <linux/slab.h> /* kmalloc, kfree */

static volatile u32 *kmalloc_ptr;
static volatile u32 static_var;

static struct dentry *debugfs_file;

static int show(struct seq_file *m, void *v)
{
	seq_printf(m,
		"*kmalloc_ptr = 0x%llx\n"
		"kmalloc_ptr = %px\n"
		"virt_to_phys(kmalloc_ptr) = 0x%llx\n"
		"static_var = 0x%llx\n"
		"&static_var = %px\n"
		"virt_to_phys(&static_var) = 0x%llx\n",
		(unsigned long long)*kmalloc_ptr,
		kmalloc_ptr,
		(unsigned long long)virt_to_phys((void *)kmalloc_ptr),
		(unsigned long long)static_var,
		&static_var,
		(unsigned long long)virt_to_phys((void *)&static_var)
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
	kmalloc_ptr = kmalloc(sizeof(kmalloc_ptr), GFP_KERNEL);
	*kmalloc_ptr = 0x12345678;
	static_var = 0x12345678;
	debugfs_file = debugfs_create_file("lkmc_virt_to_phys", S_IRUSR, NULL, NULL, &fops);
	return 0;
}

static void myexit(void)
{
	debugfs_remove(debugfs_file);
	kfree((void *)kmalloc_ptr);
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
