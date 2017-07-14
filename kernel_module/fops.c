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

#include <asm/uaccess.h> /* copy_from_user, copy_to_user */
#include <linux/debugfs.h>
#include <linux/errno.h> /* EFAULT */
#include <linux/fs.h> /* file_operations */
#include <linux/kernel.h> /* min */
#include <linux/module.h>
#include <linux/printk.h> /* printk */
#include <uapi/linux/stat.h> /* S_IRUSR */

static struct dentry *debugfs_file;
static char data[] = {'a', 'b', 'c', 'd'};

static int open(struct inode *inode, struct file *filp)
{
	pr_info("open\n");
	return 0;
}

/* @param[in,out] off: gives the initial position into the buffer.
 *      We must increment this by the ammount of bytes read.
 *      Then when userland reads the same file descriptor again,
 *      we start from that point instead.
 * */
static ssize_t read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	ssize_t ret;

	pr_info("read\n");
	pr_info("len = %zu\n", len);
	pr_info("off = %lld\n", (long long)*off);
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
	pr_info("buf = %.*s\n", (int)len, buf);
	pr_info("ret = %lld\n", (long long)ret);
	return ret;
}

/* Similar to read, but with one notable difference:
 * we must return ENOSPC if the user tries to write more
 * than the size of our buffer. Otherwise, Bash > just
 * keeps trying to write to it infinitely. */
static ssize_t write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	ssize_t ret;

	pr_info("write\n");
	pr_info("len = %zu\n", len);
	pr_info("off = %lld\n", (long long)*off);
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
				pr_info("buf = %.*s\n", (int)len, data + *off);
				*off += ret;
			}
		}
	}
	pr_info("ret = %lld\n", (long long)ret);
	return ret;
}

/*
Called on the last close:
http://stackoverflow.com/questions/11393674/why-is-the-close-function-is-called-release-in-struct-file-operations-in-the-l
*/
static int release(struct inode *inode, struct file *filp)
{
	pr_info("release\n");
	return 0;
}

static loff_t llseek(struct file *filp, loff_t off, int whence)
{
	loff_t newpos;

	pr_info("llseek\n");
	pr_info("off = %lld\n", (long long)off);
	pr_info("whence = %lld\n", (long long)whence);
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
	pr_info("newpos = %lld\n", (long long)newpos);
	return newpos;
}

static const struct file_operations fops = {
	/* Prevents rmmod while fops are running.
	 * Try removing this for poll, which waits a lot. */
	.owner = THIS_MODULE,
	.llseek = llseek,
	.open = open,
	.read = read,
	.release = release,
	.write = write,
};

static int myinit(void)
{
	debugfs_file = debugfs_create_file("lkmc_fops", S_IRUSR | S_IWUSR, NULL, NULL, &fops);
	return 0;
}

static void myexit(void)
{
	debugfs_remove_recursive(debugfs_file);
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
