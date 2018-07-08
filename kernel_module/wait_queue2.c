/* https://github.com/cirosantilli/linux-kernel-module-cheat#wait-queues */

#include <linux/delay.h> /* usleep_range */
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/wait.h> /* wait_queue_head_t, wait_event_interruptible, wake_up_interruptible  */

static struct task_struct *kthread_wake;
static struct task_struct *kthread_sleep1;
static struct task_struct *kthread_sleep2;
static wait_queue_head_t queue;
static atomic_t awake1 = ATOMIC_INIT(0);
static atomic_t awake2 = ATOMIC_INIT(0);

static int kthread_wake_func(void *data)
{
	unsigned int i = 0;
	while (!kthread_should_stop()) {
		pr_info("0 %u\n", i);
		usleep_range(1000000, 1000001);
		atomic_set(&awake1, 1);
		atomic_set(&awake2, 1);
		wake_up(&queue);
		i++;
	}
	return 0;
}

static int kthread_sleep_func_1(void *data)
{
	unsigned int i = 0;
	while (!kthread_should_stop()) {
		pr_info("1 %u\n", i);
		i++;
		wait_event(queue, atomic_read(&awake1));
		atomic_set(&awake1, 0);
		schedule();
	}
	return 0;
}

static int kthread_sleep_func_2(void *data)
{
	unsigned int i = 0;
	while (!kthread_should_stop()) {
		pr_info("2 %u\n", i);
		i++;
		wait_event(queue, atomic_read(&awake2));
		atomic_set(&awake2, 0);
		schedule();
	}
	return 0;
}

int init_module(void)
{
	init_waitqueue_head(&queue);
	kthread_wake = kthread_create(kthread_wake_func, NULL, "wake");
	kthread_sleep1 = kthread_create(kthread_sleep_func_1, NULL, "sleep1");
	kthread_sleep2 = kthread_create(kthread_sleep_func_2, NULL, "sleep2");
	wake_up_process(kthread_wake);
	wake_up_process(kthread_sleep1);
	wake_up_process(kthread_sleep2);
	return 0;
}

void cleanup_module(void)
{
	kthread_stop(kthread_sleep2);
	kthread_stop(kthread_sleep1);
	kthread_stop(kthread_wake);
}
MODULE_LICENSE("GPL");
