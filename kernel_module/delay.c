/*
Usage:

	insmod /delay.ko
	rmmod delay

dmesg prints an integer every second until rmmod.

Since insmod returns, this Illustrates how the work queues are asynchronous.
*/

#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/workqueue.h>

MODULE_LICENSE("GPL");

static struct workqueue_struct *queue;

static void work_func(struct work_struct *work)
{
	int i = 0;
	while (1) {
		printk(KERN_INFO "%d\n", i);
		usleep_range(1000000, 1000001);
		i++;
		if (i == 10)
			i = 0;
	}
}

DECLARE_WORK(work, work_func);

int init_module(void)
{
	queue = create_workqueue("myworkqueue");
	queue_work(queue, &work);
	return 0;
}

void cleanup_module(void)
{
	/* This waits for the work to finish. From docstring: */
	/* > Cancel @work and wait for its execution to finish. */
	cancel_work_sync(&work);

	destroy_workqueue(queue);
}
