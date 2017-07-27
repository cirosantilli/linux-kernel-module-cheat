/*
Remember: mmap does not work with debugfs as of 4.9!
*/

#if 0

/*
Adapted from:
https://coherentmusings.wordpress.com/2014/06/10/implementing-mmap-for-transferring-data-from-user-space-to-kernel-space/
*/

#include <linux/debugfs.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/slab.h>

#ifndef VM_RESERVED
# define  VM_RESERVED (VM_DONTEXPAND | VM_DONTDUMP)
#endif

static struct dentry *debugfs_file;

struct mmap_info
{
	char *data;
	int reference;
};

static void vm_close(struct vm_area_struct *vma)
{
	struct mmap_info *info;

	pr_info("vm_close\n");
	info = (struct mmap_info *)vma->vm_private_data;
	info->reference--;
}

static int vm_fault(struct vm_area_struct *vma, struct vm_fault *vmf)
{
	struct page *page;
	struct mmap_info *info;

	pr_info("vm_fault\n");
	info = (struct mmap_info *)vma->vm_private_data;
	if (info->data) {
		page = virt_to_page(info->data);
		get_page(page);
		vmf->page = page;
	}
	return 0;
}

static void vm_open(struct vm_area_struct *vma)
{
	struct mmap_info *info;

	pr_info("vm_open\n");
	info = (struct mmap_info *)vma->vm_private_data;
	info->reference++;
}

static struct vm_operations_struct vm_ops =
{
	.close = vm_close,
	.fault = vm_fault,
	.open = vm_open,
};

static int mmap(struct file *filp, struct vm_area_struct *vma)
{
	pr_info("mmap\n");
	vma->vm_ops = &vm_ops;
	vma->vm_flags |= VM_RESERVED;
	vma->vm_private_data = filp->private_data;
	vm_open(vma);
	return 0;
}

static int open(struct inode *inode, struct file *filp)
{
	struct mmap_info *info;

	pr_info("open\n");
	info = kmalloc(sizeof(struct mmap_info), GFP_KERNEL);
	info->data = (char *)get_zeroed_page(GFP_KERNEL);
	memcpy(info->data, "abc", 4);
	filp->private_data = info;
	return 0;
}

static int release(struct inode *inode, struct file *filp)
{
	struct mmap_info *info;

	pr_info("release\n");
	info = filp->private_data;
	free_page((unsigned long)info->data);
	kfree(info);
	filp->private_data = NULL;
	return 0;
}

static const struct file_operations fops = {
	.mmap = mmap,
	.open = open,
	.release = release,
};

static int myinit(void)
{
	debugfs_file = debugfs_create_file("lkmc_mmap", S_IRWXU, NULL, NULL, &fops);
	return 0;
}

static void myexit(void)
{
	debugfs_remove(debugfs_file);
}

module_init(myinit);
module_exit(myexit);
MODULE_LICENSE("GPL");


/*minimized debugfs*/


#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/mm.h>

static struct dentry *debugfs_file;

static int mmap(struct file *filp, struct vm_area_struct *vma)
{
	pr_info("mmap\n");
	if (remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff,
				vma->vm_end - vma->vm_start, vma->vm_page_prot)) {
		return -EAGAIN;
	}
	return 0;
}

struct file_operations fops =
{
	.owner = THIS_MODULE,
	.open    = nonseekable_open,
	.mmap    = mmap
};

static int myinit(void)
{
	debugfs_file = debugfs_create_file("lkmc_mmap", S_IRWXU, NULL, NULL, &fops);
	return 0;
}

static void myexit(void)
{
	debugfs_remove(debugfs_file);
}

module_init(myinit);
module_exit(myexit);
MODULE_LICENSE("GPL");

/*working chardev*/

#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/mm.h>

#define NAME "lkmc_mmap"

static int major;

static int mmap(struct file *filp, struct vm_area_struct *vma)
{
	pr_info("mmap\n");
	if (remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff,
				vma->vm_end - vma->vm_start, vma->vm_page_prot)) {
		return -EAGAIN;
	}
	return 0;
}

struct file_operations fops =
{
	.owner = THIS_MODULE,
	.open    = nonseekable_open,
	.mmap    = mmap
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

#endif

/* proc attempt */

#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/proc_fs.h>

static const char *filename = "lkmc_procfs";

static int mmap(struct file *filp, struct vm_area_struct *vma)
{
	pr_info("mmap\n");
	return 0;
}

struct file_operations fops =
{
	.owner = THIS_MODULE,
	.open = nonseekable_open,
	.mmap = mmap
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
