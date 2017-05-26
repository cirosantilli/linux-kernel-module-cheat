/*
Basic fops example, with a fixed size static data buffer.

Usage:

	/fops.sh

The buffer can be written and read from. If data overflows, data is thrown away.

No, there ain't no official docs:
http://stackoverflow.com/questions/15213932/what-are-the-struct-file-operations-arguments

fops define what the kernel will do on filesystem system calls on all of
/dev, /proc, /sys, and consistute the main method of userland communication
in drivers (syscalls being the other one).

Here we use debugfs.
*/

#include <linux/debugfs.h>
#include <linux/errno.h> /* EFAULT */
#include <linux/fs.h>
#include <linux/kernel.h> /* min */
#include <linux/module.h>
#include <linux/printk.h> /* printk */
#include <asm/uaccess.h> /* copy_from_user, copy_to_user */

MODULE_LICENSE("GPL");

static struct dentry *dir;

static char data[] = {'a', 'b', 'c', 'd'};

static int fop_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "open\n");
	return 0;
}

/* @param[in,out] off: gives the initial position into the buffer.
 *      We must increment this by the ammount of bytes read.
 *      Then when userland reads the same file descriptor again,
 *      we start from that point instead.
 * */
static ssize_t fop_read(struct file *file, char __user *buf, size_t len, loff_t *off)
{
	ssize_t ret;
	printk(KERN_INFO "read\n");
	printk(KERN_INFO "len = %zu\n", len);
	printk(KERN_INFO "off = %lld\n", (long long)*off);
	if (sizeof(data) <= *off) {
		ret = 0;
	} else {
		ret = min(len, sizeof(data) - (size_t)*off);
		if (copy_to_user(buf, data + *off, ret)) {
			ret = -EFAULT;
		} else {
			*off += ret;
		}
	}
	printk(KERN_INFO "buf = %.*s\n", (int)len, buf);
	printk(KERN_INFO "ret = %lld\n", (long long)ret);
	return ret;
}

/* Similar to read, but with one notable difference:
 * we must return ENOSPC if the user tries to write more
 * than the size of our buffer. Otherwise, Bash > just
 * keeps trying to write to it infinitely. */
static ssize_t fop_write(struct file *file, const char __user *buf, size_t len, loff_t *off)
{
	ssize_t ret;
	printk(KERN_INFO "write\n");
	printk(KERN_INFO "len = %zu\n", len);
	printk(KERN_INFO "off = %lld\n", (long long)*off);
	if (sizeof(data) <= *off) {
		ret = 0;
	} else {
		if (sizeof(data) - (size_t)*off < len) {
			ret = -ENOSPC;
		} else {
			if (copy_from_user(data + *off, buf, len)) {
				ret = -EFAULT;
			} else {
				ret = len;
				printk(KERN_INFO "buf = %.*s\n", (int)len, data + *off);
				*off += ret;
			}
		}
	}
	printk(KERN_INFO "ret = %lld\n", (long long)ret);
	return ret;
}

/*
Called on the last close:
http://stackoverflow.com/questions/11393674/why-is-the-close-function-is-called-release-in-struct-file-operations-in-the-l
*/
static int fop_release (struct inode *inode, struct file *file)
{
	printk(KERN_INFO "release\n");
	return 0;
}

static loff_t fop_llseek(struct file *filp, loff_t off, int whence)
{
	loff_t newpos;
	printk(KERN_INFO "llseek\n");
	printk(KERN_INFO "off = %lld\n", (long long)off);
	printk(KERN_INFO "whence = %lld\n", (long long)whence);

	switch(whence) {
		case SEEK_SET:
			newpos = off;
			break;
		case SEEK_CUR:
			newpos = filp->f_pos + off;
			break;
		case SEEK_END:
			newpos = sizeof(data) + off;
			break;
		default:
			return -EINVAL;
	}
	if (newpos < 0) return -EINVAL;
	filp->f_pos = newpos;
	printk(KERN_INFO "newpos = %lld\n", (long long)newpos);
	return newpos;
}

static const struct file_operations fops = {
	.llseek = fop_llseek,
	.open = fop_open,
	.read = fop_read,
	.release = fop_release,
	.write = fop_write,
};

int init_module(void)
{
	struct dentry *file;
	dir = debugfs_create_dir("lkmc_fops", 0);
	if (!dir) {
		printk(KERN_ALERT "debugfs_create_dir failed");
		return -1;
	}
    file = debugfs_create_file("f", 0666, dir, NULL, &fops);
	if (!file) {
		printk(KERN_ALERT "debugfs_create_file failed");
		return -1;
	}
	return 0;
}

void cleanup_module(void)
{
	debugfs_remove_recursive(dir);
}
