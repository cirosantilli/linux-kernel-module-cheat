/*
Declare more work from a workqueue.

TODO: kernel panic. Why?
*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/workqueue.h>

MODULE_LICENSE("GPL");

static int i = 0;
static struct workqueue_struct *queue;

static void work_func(struct work_struct *work)
{
	DECLARE_DELAYED_WORK(next_work, work_func);
	printk(KERN_INFO "%d\n", i);
	i++;
	queue_delayed_work(queue, &next_work, HZ);
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
	destroy_workqueue(queue);
}
