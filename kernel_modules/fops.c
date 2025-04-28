/* https://cirosantilli.com/linux-kernel-module-cheat#fops */

#include <linux/debugfs.h>
#include <linux/errno.h> /* EFAULT */
#include <linux/fs.h> /* file_operations */
#include <linux/kernel.h> /* min */
#include <linux/module.h>
#include <linux/printk.h> /* printk */
#include <linux/uaccess.h> /* copy_from_user, copy_to_user */
#include <uapi/linux/stat.h> /* S_IRUSR */

static struct dentry *debugfs_file;
// The buffer can be stored in two ways: static module data or kmalloc.
#define STATIC 1
#if STATIC
static char data[] = {'a', 'b', 'c', 'd'};
#define BUFLEN sizeof(data)
#else
static char *data;
#define BUFLEN 4
#endif

static int open(struct inode *inode, struct file *filp)
{
	pr_info("open\n");
	return 0;
}

/* @param[in,out] off: gives the initial position into the buffer.
 *      We must increment this by the amount of bytes read.
 *      Then when userland reads the same file descriptor again,
 *      we start from that point instead.
 */
static ssize_t read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	ssize_t ret;

	pr_info("read len=%zu off=%lld\n", len, (long long)*off);
	if (BUFLEN <= *off) {
		ret = 0;
	} else {
		ret = min(len, BUFLEN - (size_t)*off);
		if (copy_to_user(buf, data + *off, ret)) {
			ret = -EFAULT;
		} else {
			*off += ret;
		}
	}
	pr_info("ret=%lld\n", (long long)ret);
	return ret;
}

/* Similar to read, but with one notable difference:
 * we must return ENOSPC if the user tries to write more
 * than the size of our buffer. Otherwise, Bash > just
 * keeps trying to write to it infinitely.
 */
static ssize_t write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	ssize_t ret;

	pr_info("write len=%zu off=%lld\n", len, (long long)*off);
	if (BUFLEN <= *off) {
		ret = 0;
	} else {
		if (BUFLEN - (size_t)*off < len) {
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

/* Called on the last close:
 * http://stackoverflow.com/questions/11393674/why-is-the-close-function-is-called-release-in-struct-file-operations-in-the-l
 */
static int release(struct inode *inode, struct file *filp)
{
	pr_info("release\n");
	return 0;
}

static loff_t llseek(struct file *filp, loff_t off, int whence)
{
	loff_t newpos;

	pr_info("llseek off=%lld whence=%lld\n", (long long)off, (long long)whence);
	switch(whence) {
		case SEEK_SET:
			newpos = off;
			break;
		case SEEK_CUR:
			newpos = filp->f_pos + off;
			break;
		case SEEK_END:
			newpos = BUFLEN + off;
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
#if STATIC == 0
	data = kmalloc(BUFLEN, GFP_KERNEL);
	if (!data)
		return -ENOMEM;
	data[0] = 'a';
	data[1] = 'b';
	data[2] = 'c';
	data[3] = 'd';
#endif
	debugfs_file = debugfs_create_file("lkmc_fops", S_IRUSR | S_IWUSR, NULL, NULL, &fops);
	return 0;
}

static void myexit(void)
{
#if STATIC == 0
	kfree(data);
#endif
	debugfs_remove_recursive(debugfs_file);
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
