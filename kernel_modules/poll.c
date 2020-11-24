/* https://cirosantilli.com/linux-kernel-module-cheat#poll */

#include <linux/debugfs.h>
#include <linux/delay.h> /* usleep_range */
#include <linux/errno.h> /* EFAULT */
#include <linux/fs.h>
#include <linux/jiffies.h>
#include <linux/kernel.h> /* min */
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/poll.h>
#include <linux/printk.h> /* printk */
#include <linux/uaccess.h> /* copy_from_user, copy_to_user */
#include <linux/wait.h> /* wait_queue_head_t, wait_event_interruptible, wake_up_interruptible  */
#include <uapi/linux/stat.h> /* S_IRUSR */

static int ret0 = 0;
module_param(ret0, int, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(i, "if 1, always return 0 from poll");

static char readbuf[1024];
static size_t readbuflen;
static struct dentry *debugfs_file;
static struct task_struct *kthread;
static wait_queue_head_t waitqueue;

static ssize_t read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	ssize_t ret;
	if (copy_to_user(buf, readbuf, readbuflen)) {
		ret = -EFAULT;
	} else {
		ret = readbuflen;
	}
	/* This is normal pipe behaviour: data gets drained once a reader reads from it. */
	/* https://stackoverflow.com/questions/1634580/named-pipes-fifos-on-unix-with-multiple-readers */
	readbuflen = 0;
	return ret;
}

/* If you return 0 here, then the kernel will sleep until an event
 * happens in the queue. and then call this again, because of the call to poll_wait. */
unsigned int poll(struct file *filp, struct poll_table_struct *wait)
{
	pr_info("poll\n");
	/* This doesn't sleep. It just makes the kernel call poll again if we return 0. */
	poll_wait(filp, &waitqueue, wait);
	if (readbuflen && !ret0) {
		pr_info("return POLLIN\n");
		return POLLIN;
	} else {
		pr_info("return 0\n");
		return 0;
	}
}

static int lkmc_kthread_func(void *data)
{
	while (!kthread_should_stop()) {
		readbuflen = snprintf(
			readbuf,
			sizeof(readbuf),
			"%llu",
			(unsigned long long)jiffies
		);
		usleep_range(1000000, 1000001);
		pr_info("wake_up\n");
		wake_up(&waitqueue);
	}
	return 0;
}

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = read,
	.poll = poll
};

static int myinit(void)
{
	debugfs_file = debugfs_create_file(
		"lkmc_poll", S_IRUSR | S_IWUSR, NULL, NULL, &fops);
	init_waitqueue_head(&waitqueue);
	kthread = kthread_create(lkmc_kthread_func, NULL, "mykthread");
	wake_up_process(kthread);
	return 0;
}

static void myexit(void)
{
	kthread_stop(kthread);
	debugfs_remove(debugfs_file);
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
