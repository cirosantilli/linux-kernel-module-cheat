/* https://github.com/cirosantilli/linux-kernel-module-cheat#arm-pmccntr */

#include <linux/debugfs.h>
#include <linux/errno.h> /* EFAULT */
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/printk.h> /* pr_info */
#include <linux/seq_file.h> /* seq_read, seq_lseek, single_release */
#include <linux/uaccess.h> /* copy_from_user, copy_to_user */
#include <uapi/linux/stat.h> /* S_IRUSR */

static struct dentry *debugfs_file;

static int show(struct seq_file *m, void *v)
{
	u32 pmccntr;
#if defined(__arm__)
	/* Invalid aarch64 asm. */

	/* TODO  Internal error: Oops - undefined instruction: 0 [#1] ARM */
	/* Enable userland access to conter. */
	/* PMUSERENR = 1 */
	/*__asm__ __volatile__ ("mcr p15, 0, %0, c9, c14, 0" :: "r"(1));*/

	/* TODO oops undefined instruction. */
	/* PMCR.E (bit 0) = 1 */
	/*__asm__ __volatile__ ("mcr p15, 0, %0, c9, c12, 0" :: "r"(1));*/

	/* TODO oops undefined instruction. */
	/* Enable counter. */
	/* PMCNTENSET.C (bit 31) = 1 */
	/*__asm__ __volatile__ ("mcr p15, 0, %0, c9, c12, 1" :: "r"(1 << 31));*/

	/* Get counter value. */
	__asm__ __volatile__ ("mrc p15, 0, %0, c15, c12, 1" : "=r" (pmccntr));
#else
	pmccntr = 0;
#endif
	seq_printf(m, "%8.8llX\n", (unsigned long long)pmccntr);
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
	debugfs_file = debugfs_create_file("lkmc_pmccntr", S_IRUSR, NULL, NULL, &fops);
	if (!debugfs_file) {
		return -1;
	}
	return 0;
}

static void myexit(void)
{
	debugfs_remove(debugfs_file);
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
