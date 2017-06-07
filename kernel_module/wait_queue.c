/*
"wait_event" works a bit like:

	if (!cond)
		sleep_until_event

Outcome:

	1 0
	2 0
	# Wait one second.
	1 1
	2 1
	# Wait one second.
	1 2
	2 2
	# ...
*/

#include <linux/delay.h> /* usleep_range */
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/wait.h> /* wait_queue_head_t, wait_event_interruptible, wake_up_interruptible  */

MODULE_LICENSE("GPL");

static struct task_struct *kthread1, *kthread2;
static wait_queue_head_t queue;
static atomic_t awake = ATOMIC_INIT(0);

static int kthread_func1(void *data)
{
	unsigned int i = 0;
	while (!kthread_should_stop()) {
		pr_info("1 %u\n", i);
		usleep_range(1000000, 1000001);
		atomic_set(&awake, 1);
		wake_up(&queue);
		i++;
	}
	return 0;
}

static int kthread_func2(void *data)
{
	unsigned int i = 0;
	while (!kthread_should_stop()) {
		pr_info("2 %u\n", i);
		i++;
		wait_event(queue, atomic_read(&awake));
		atomic_set(&awake, 0);
		schedule();
	}
	return 0;
}

static int myinit(void)
{
	init_waitqueue_head(&queue);
	kthread1 = kthread_create(kthread_func1, NULL, "mykthread1");
	kthread2 = kthread_create(kthread_func2, NULL, "mykthread2");
	wake_up_process(kthread1);
	wake_up_process(kthread2);
	return 0;
}

static void myexit(void)
{
	/* 2 must be stopped before, or else we can deadlock. */
	kthread_stop(kthread2);
	kthread_stop(kthread1);
}

module_init(myinit)
module_exit(myexit)
