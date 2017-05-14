/*
Usage:

	insmod /workqueue.ko
	# dmesg => worker
	rmmod workqueue

Creates a separate thread. So init_module can return, but some work will still get done.

TODO why can't call this workqueue.ko?
https://unix.stackexchange.com/questions/364956/how-can-insmod-fail-with-kernel-module-is-already-loaded-even-is-lsmod-does-not
*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/workqueue.h>

MODULE_LICENSE("GPL");

static struct workqueue_struct *queue;

static void worker_func(struct work_struct *work)
{
	printk(KERN_INFO "worker\n");
}

int init_module(void)
{
	DECLARE_WORK(work, worker_func);
	queue = create_singlethread_workqueue("myworkqueue");
	queue_work(queue, &work);
	return 0;
}

void cleanup_module(void)
{
	destroy_workqueue(queue);
}
