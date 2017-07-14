/*
Usage:

	insmod /workqueue_cheat.ko
	# dmesg => worker
	rmmod workqueue_cheat

Creates a separate thread. So init_module can return, but some work will still get done.

Can't call this just workqueue.c because there is already a built-in with that name:
https://unix.stackexchange.com/questions/364956/how-can-insmod-fail-with-kernel-module-is-already-loaded-even-is-lsmod-does-not

Workqueues are a convenience frontend for kthreads.

Bibliography:

- https://www.ibm.com/developerworks/library/l-tasklets/
*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/workqueue.h>

static struct workqueue_struct *queue;

static void work_func(struct work_struct *work)
{
	pr_info("worker\n");
}

DECLARE_WORK(work, work_func);

static int myinit(void)
{
	queue = create_singlethread_workqueue("myworkqueue");
	queue_work(queue, &work);
	return 0;
}

static void myexit(void)
{
	/* Waits for jobs to finish. */
	destroy_workqueue(queue);
}

module_init(myinit)
module_exit(myexit)
MODULE_LICENSE("GPL");
