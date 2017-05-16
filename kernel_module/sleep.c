/*
Usage:

	insmod /sleep.ko
	rmmod sleep

dmesg prints an integer every second until rmmod.

Since insmod returns, this also illustrates how the work queues are asynchronous.
*/

#include <linux/delay.h> /* usleep_range */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h> /* atomic_t */
#include <linux/workqueue.h>

MODULE_LICENSE("GPL");

static struct workqueue_struct *queue;
static atomic_t run = ATOMIC_INIT(1);

static void work_func(struct work_struct *work)
{
	int i = 0;
	while (atomic_read(&run)) {
		printk(KERN_INFO "%d\n", i);
		usleep_range(1000000, 1000001);
		i++;
		if (i == 10)
			i = 0;
	}
}

int init_module(void)
{
	DECLARE_WORK(work, work_func);
	queue = create_workqueue("myworkqueue");
	queue_work(queue, &work);
	return 0;
}

void cleanup_module(void)
{
	atomic_set(&run, 0);
	destroy_workqueue(queue);
}
