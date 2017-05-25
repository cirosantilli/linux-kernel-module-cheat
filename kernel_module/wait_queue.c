/*
TODO get working. Thread 2 only wakes up once! Wake thread 2 up every 2 seconds from thread 1.
*/

#include <linux/delay.h> /* usleep_range */
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/wait.h> /* wait_queue_head_t, wait_event_interruptible, wake_up_interruptible  */

MODULE_LICENSE("GPL");

static struct task_struct *kthread1, *kthread2;
static wait_queue_head_t queue;
static int awake = 0;

static int kthread_func1(void *data)
{
	int i = 0;
	while (!kthread_should_stop()) {
		awake = !awake;
		pr_info("1 %d\n", i);
		wake_up_interruptible(&queue);
		schedule();
		usleep_range(1000000, 1000001);
		i++;
		if (i == 10)
			i = 0;
	}
	i = !i;
	wake_up_interruptible(&queue);
	return 0;
}

static int kthread_func2(void *data)
{
	set_current_state(TASK_INTERRUPTIBLE);
	int i = 0;
	while (!kthread_should_stop()) {
		wait_event(queue, awake);
		pr_info("2 %d\n", i);
		set_current_state(TASK_INTERRUPTIBLE);
		schedule();
		i++;
		if (i == 10)
			i = 0;
	}
	return 0;
}

int init_module(void)
{
	init_waitqueue_head(&queue);
	kthread1 = kthread_create(kthread_func1, NULL, "mykthread1");
	kthread2 = kthread_create(kthread_func2, NULL, "mykthread2");
	wake_up_process(kthread1);
	wake_up_process(kthread2);
	return 0;
}

void cleanup_module(void)
{
	kthread_stop(kthread1);
	kthread_stop(kthread2);
}
