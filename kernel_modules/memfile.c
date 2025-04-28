/* https://cirosantilli.com/linux-kernel-module-cheat#fops */

#include <linux/debugfs.h>
#include <linux/errno.h> /* EFAULT */
#include <linux/fs.h> /* file_operations */
#include <linux/kernel.h> /* min */
#include <linux/module.h>
#include <linux/printk.h> /* printk */
#include <linux/string.h> /* strcpy */
#include <linux/uaccess.h> /* copy_from_user, copy_to_user */
#include <linux/rwsem.h>
#include <uapi/linux/stat.h> /* S_IRUSR */

/* Params */
static int log = 0;
module_param(log, int, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(log, "enable logging");

/* Dynamic array: https://stackoverflow.com/questions/3536153/c-dynamically-growing-array */

typedef struct {
	char *buf;
	size_t used;
	size_t _size;
} dyn_arr_t;

int dyn_arr_init(dyn_arr_t *a, size_t size);
int dyn_arr_init(dyn_arr_t *a, size_t size)
{
	a->buf = kvzalloc(size, GFP_KERNEL);
	if (!a->buf)
		return -ENOMEM;
	a->used = 0;
	a->_size = size;
	return 0;
}

/* Reserve the required space for a future data insertion of size len at offset off.
 * We don't do the actual insertion here as there are multiple possible insertion methods
 * e.g. copy_from_user or strcpy.
 */
int dyn_arr_reserve(dyn_arr_t *a, size_t off, size_t len);
int dyn_arr_reserve(dyn_arr_t *a, size_t off, size_t len)
{
	size_t new_used, new_size;
	
	new_used = off + len;
	if (new_used > a->_size) {
		new_size = new_used * 2;
		a->buf = kvrealloc(a->buf, a->_size, new_size, GFP_KERNEL);
		if (!a->buf)
			return -ENOMEM;
		a->_size = new_size;
	}
	if (off > a->used)
		memset(a->buf + a->used, '\0', off - a->used);
	if (new_used > a->used)
		a->used = new_used;
	if (log) pr_info("dyn_arr_reserve _size:=%zu used:=%zu\n", a->_size, a->used);
	return 0;
}

void dyn_arr_free(dyn_arr_t *a);
void dyn_arr_free(dyn_arr_t *a)
{
	kvfree(a->buf);
	a->buf = NULL;
	a->used = 0;
	a->_size = 0;
}

/* Globals. */
static dyn_arr_t data;
static struct dentry *debugfs_file;
struct rw_semaphore rwsem;

static int open(struct inode *inode, struct file *filp)
{
	if (log) pr_info("open\n");
	if ((filp->f_flags & O_TRUNC)) {
		if (log) pr_info("open O_TRUNC\n");
		data.used = 0;
	} else if ((filp->f_flags & O_APPEND)) {
		if (log) pr_info("open O_APPEND\n");
		filp->f_pos = data.used;
	}
	return 0;
}

static ssize_t read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	ssize_t ret;

	if (log) pr_info("read len=%zu off=%lld\n", len, (long long)*off);
	down_read(&rwsem);
	if (data.used <= *off) {
		ret = 0;
	} else {
		ret = min(len, data.used - (size_t)*off);
		if (copy_to_user(buf, data.buf + *off, ret)) {
			ret = -EFAULT;
		} else {
			*off += ret;
		}
	}
	up_read(&rwsem);
	if (log) pr_info("read ret:=%lld\n", (long long)ret);
	return ret;
}

static ssize_t write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	ssize_t ret;

	if (log) pr_info("write len=%zu off=%lld\n", len, (long long)*off);
	down_write(&rwsem);
	dyn_arr_reserve(&data, *off, len);
	if (copy_from_user(data.buf + *off, buf, len)) {
		ret = -EFAULT;
	} else {
		ret = len;
		*off += ret;
	}
	up_write(&rwsem);
	if (log) pr_info("write ret:=%lld\n", (long long)ret);
	return ret;
}

static int release(struct inode *inode, struct file *filp)
{
	if (log) pr_info("release\n");
	return 0;
}

static loff_t llseek(struct file *filp, loff_t off, int whence)
{
	loff_t newpos;

	if (log) pr_info("llseek off=%lld whence=%lld\n", (long long)off, (long long)whence);
	switch(whence) {
		case SEEK_SET:
			newpos = off;
			break;
		case SEEK_CUR:
			newpos = filp->f_pos + off;
			break;
		case SEEK_END:
			newpos = data.used + off;
			break;
		default:
			return -EINVAL;
	}
	if (newpos < 0) return -EINVAL;
	filp->f_pos = newpos;
	if (log) pr_info("llseek newpos:=%lld\n", (long long)newpos);
	return newpos;
}

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.llseek = llseek,
	.open = open,
	.read = read,
	.release = release,
	.write = write,
};

static int myinit(void)
{
	int ret;
	
	ret = dyn_arr_init(&data, 1);
	if (ret)
		return ret;
	init_rwsem(&rwsem);
	debugfs_file = debugfs_create_file("lkmc_memfile",
		S_IRUSR | S_IWUSR, NULL, NULL, &fops);
	return 0;
}

static void myexit(void)
{
	dyn_arr_free(&data);
	debugfs_remove_recursive(debugfs_file);
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
