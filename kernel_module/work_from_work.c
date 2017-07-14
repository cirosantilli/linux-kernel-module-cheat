/*
Declare more work from a workqueue.
*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/workqueue.h>

MODULE_LICENSE("GPL");

static int i = 0;
static struct workqueue_struct *queue;

static void work_func(struct work_struct *work);

DECLARE_DELAYED_WORK(next_work, work_func);
DECLARE_WORK(work, work_func);

static void work_func(struct work_struct *work)
{
	pr_info("%d\n", i);
	i++;
	queue_delayed_work(queue, &next_work, HZ);
}

static int myinit(void)
{
	queue = create_workqueue("myworkqueue");
	queue_work(queue, &work);
	return 0;
}

static void myexit(void)
{
	cancel_delayed_work(&next_work);
	flush_workqueue(queue);
	destroy_workqueue(queue);
}

module_init(myinit)
module_exit(myexit)
