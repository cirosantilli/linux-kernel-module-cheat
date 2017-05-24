/*
Basic poll file_operation example.

Waits for a second, give jiffies to user, wait for a second...

usleep_range
*/

#include <asm/uaccess.h> /* copy_from_user, copy_to_user */
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

MODULE_LICENSE("GPL");

static struct dentry *dir;
static struct task_struct *kthread;
static wait_queue_head_t waitqueue;

static ssize_t read(struct file *file, char __user *buf, size_t len, loff_t *off)
{
	ssize_t ret;
	char s[1024];

	ret = snprintf(s, sizeof(s), "%llu", (unsigned long long)jiffies);
	if (copy_to_user(buf, s, ret)) {
		ret = -EFAULT;
	}
	return ret;
}

unsigned int poll(struct file *filp, struct poll_table_struct *wait)
{
	/*TODO*/
	/*wait_event_interruptible(waitqueue, (dev->rp != dev->wp));*/
	pr_info("poll_wait before\n");
	poll_wait(filp, &waitqueue, wait);
	pr_info("poll_wait after\n");
	return POLLIN;
}

static int kthread_func(void *data)
{
	while (!kthread_should_stop()) {
		usleep_range(1000000, 1000001);
		wake_up_interruptible(&waitqueue);
	}
	return 0;
}

static const struct file_operations fops = {
	.read = read,
	.poll = poll
};

int init_module(void)
{
	dir = debugfs_create_dir("kernel_module_cheat_poll", 0);
    debugfs_create_file("f", 0666, dir, NULL, &fops);
	init_waitqueue_head(&waitqueue);
	kthread = kthread_create(kthread_func, NULL, "mykthread");
	return 0;
}

void cleanup_module(void)
{
	debugfs_remove_recursive(dir);
}
